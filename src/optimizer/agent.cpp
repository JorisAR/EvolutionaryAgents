#include "agent.h"

using namespace godot;

void EvolutionaryAgent::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("infer"), &EvolutionaryAgent::infer);
    ClassDB::bind_method(D_METHOD("start"), &EvolutionaryAgent::start);

    ClassDB::bind_method(D_METHOD("set_fitness", "value"), &EvolutionaryAgent::set_fitness);
    ClassDB::bind_method(D_METHOD("get_fitness"), &EvolutionaryAgent::get_fitness);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fitness"), "set_fitness", "get_fitness");

    ClassDB::bind_method(D_METHOD("set_neural_network", "value"), &EvolutionaryAgent::set_neural_network);
    ClassDB::bind_method(D_METHOD("get_neural_network"), &EvolutionaryAgent::get_neural_network);

    ClassDB::bind_static_method("EvolutionaryAgent", D_METHOD("get_max_element_index", "array"),
                                &EvolutionaryAgent::GetMaxElementIndex);
    ClassDB::bind_static_method("EvolutionaryAgent", D_METHOD("soft_max", "array"), &EvolutionaryAgent::SoftMax);
    ClassDB::bind_static_method("EvolutionaryAgent",
                                D_METHOD("sample_weighted_index", "array", "random_sample_unit_interval"),
                                &EvolutionaryAgent::SampleWeightedIndex);
    ClassDB::bind_static_method("EvolutionaryAgent", D_METHOD("sample_normal", "mean", "sigma"),
                                &EvolutionaryAgent::SampleNormal);
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                              "NeuralNetworkParameters"),
                 "set_neural_network", "get_neural_network");

    ADD_SIGNAL(MethodInfo("started"));
    ADD_SIGNAL(MethodInfo("ended"));
}

EvolutionaryAgent::EvolutionaryAgent() : nn(nullptr)
{
}
EvolutionaryAgent::~EvolutionaryAgent()
{
    if (nn != nullptr)
        delete nn;
}

int EvolutionaryAgent::GetMaxElementIndex(const PackedFloat32Array &array)
{
    int max_index = 0;
    float max_value = array[max_index];

    for (int i = 1; i < array.size(); ++i)
    {
        if (array[i] > max_value)
        {
            max_value = array[i];
            max_index = i;
        }
    }

    return max_index;
}

PackedFloat32Array EvolutionaryAgent::SoftMax(const PackedFloat32Array &array)
{
    PackedFloat32Array exp_values;
    float sum_exp = 0.0f;

    // Calculate the exponentials of each element and sum them
    for (int i = 0; i < array.size(); ++i)
    {
        float exp_value = std::exp(array[i]);
        exp_values.append(exp_value);
        sum_exp += exp_value;
    }

    // Calculate the softmax values
    for (int i = 0; i < array.size(); ++i)
    {
        exp_values[i] = exp_values[i] / sum_exp;
    }

    return exp_values;
}

float EvolutionaryAgent::SampleNormal(const float mean, const float sigma)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(mean, sigma);
    return dist(gen);
}

int EvolutionaryAgent::SampleWeightedIndex(const PackedFloat32Array &array,
                                           const float random_sample_unit_interval = -1)
{
    float random_value = random_sample_unit_interval;
    if (random_value > 1.0f || random_value < 0.0f)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        random_value = dist(gen);
    }

    // Compute the sum of the array elements
    float array_sum = std::accumulate(array.begin(), array.end(), 0.0f);
    random_value *= array_sum;

    // Perform weighted sampling
    float cumulative_sum = 0.0f;
    for (int i = 0; i < array.size(); ++i)
    {
        cumulative_sum += array[i];
        if (random_value <= cumulative_sum)
        {
            return i;
        }
    }
    return array.size() - 1;
}

PackedFloat32Array EvolutionaryAgent::infer(const PackedFloat32Array &state_vector)
{
    if (nn == nullptr)
    {
        UtilityFunctions::printerr("Agent does not have a neural network.");
        return PackedFloat32Array();
    }
    if (nn->get_input_size() != state_vector.size())
    {
        UtilityFunctions::printerr("State vector does not equal neural network input size.");
        return Utils::vector_to_array_float(nn->get_zero_out());
    }

    std::vector<float> outputs = nn->infer(Utils::array_to_vector_float(state_vector));
    auto action_vector = Utils::vector_to_array_float(outputs);
    emit_signal("neural_network_inferred", action_vector);
    return action_vector;
}

void EvolutionaryAgent::update(const std::vector<int> &new_layers, const std::vector<float> &parameters)
{
    if (nn == nullptr)
    {
        nn = new NeuralNetwork(new_layers);
    }
    nn->update(new_layers, parameters);
}

void EvolutionaryAgent::start()
{
    if (nn == nullptr)
    {
        if (neural_network.is_null()){
            UtilityFunctions::printerr("Agent does not have a neural network, start aborted.");
            return;
        }

        auto state = EA::EvolutionaryAlgorithmState::from_json(neural_network->get_stored_network());
        if (state != nullptr)
        {
            update(neural_network->get_layers(), state->genome);
        }
    }

    emit_signal("started");
}

float EvolutionaryAgent::get_fitness() const
{
    return fitness;
}
void EvolutionaryAgent::set_fitness(const float new_fitness)
{
    fitness = new_fitness;
}

void EvolutionaryAgent::set_neural_network(const Ref<NeuralNetworkParameters> value)
{
    neural_network = value;
}

Ref<NeuralNetworkParameters> EvolutionaryAgent::get_neural_network() const
{
    return neural_network;
}