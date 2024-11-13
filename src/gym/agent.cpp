#include "agent.h"

using namespace godot;

int godot::Agent::GetMaxElementIndex(const PackedFloat32Array &array)
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

PackedFloat32Array godot::Agent::SoftMax(const PackedFloat32Array &array)
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

float godot::Agent::SampleNormal(const float mean, const float sigma)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(mean, sigma);
    return dist(gen);
}

int godot::Agent::SampleWeightedIndex(const PackedFloat32Array &array, const float random_sample_unit_interval = -1)
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

PackedFloat32Array godot::Agent::infer(const PackedFloat32Array &state_vector)
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

void Agent::update(const std::vector<int> &new_layers, const std::vector<float> &parameters)
{
    nn->update(new_layers, parameters);
}

void godot::Agent::start_game()
{
    emit_signal("started");
    _on_game_started();
}
