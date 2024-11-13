#ifndef AGENT_H
#define AGENT_H

#include "neural_network.h"
#include "neural_network_parameters.h"
#include "utils.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>
#include <numeric>
#include <random>

namespace godot
{

class Agent : public Node
{
    GDCLASS(Agent, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("_on_game_started"), &Agent::_on_game_started);
        ClassDB::bind_method(D_METHOD("infer"), &Agent::infer);
        ADD_SIGNAL(MethodInfo("neural_network_inferred", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "action_vector")));

        ClassDB::bind_method(D_METHOD("set_fitness", "value"), &Agent::set_fitness);
        ClassDB::bind_method(D_METHOD("get_fitness"), &Agent::get_fitness);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fitness"), "set_fitness", "get_fitness");

        ClassDB::bind_method(D_METHOD("set_neural_network", "value"), &Agent::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &Agent::get_neural_network);

        ClassDB::bind_static_method("Agent", D_METHOD("get_max_element_index", "array"), &Agent::GetMaxElementIndex);
        ClassDB::bind_static_method("Agent", D_METHOD("soft_max", "array"), &Agent::SoftMax);
        ClassDB::bind_static_method("Agent", D_METHOD("sample_weighted_index", "array", "random_sample_unit_interval"), &Agent::SampleWeightedIndex);
        ClassDB::bind_static_method("Agent", D_METHOD("sample_normal", "mean", "sigma"), &Agent::SampleNormal);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "NeuralNetworkParameters"),
                     "set_neural_network", "get_neural_network");

        ADD_SIGNAL(MethodInfo("started"));
        ADD_SIGNAL(MethodInfo("ended"));
    }

    Agent() : nn(new NeuralNetwork({1,1}))
    {
    }
    ~Agent()
    {
        delete nn;
    }

    static int GetMaxElementIndex(const PackedFloat32Array &array);
    static PackedFloat32Array SoftMax(const PackedFloat32Array &array);
    static float SampleNormal(const float mean, const float sigma);
    static int SampleWeightedIndex(const PackedFloat32Array &array, const float random_sample);

    PackedFloat32Array infer(const PackedFloat32Array &state_vector);
    void Agent::update(const std::vector<int> &new_layers, const std::vector<float> &parameters);
    float get_fitness() const
    {
        return fitness;
    }
    void set_fitness(const float new_fitness)
    {
        fitness = new_fitness;
    }

    void set_neural_network(const Ref<NeuralNetworkParameters> value)
    {
        neural_network = value;
        if(neural_network == nullptr) return;
        auto network = neural_network->load_neural_network_from_file();
        if(network == nullptr) return;
        delete nn;
        nn = network;
    }

    Ref<NeuralNetworkParameters> get_neural_network() const
    {
        return neural_network;
    }

    void start_game();
    void _on_game_started() {};

  private:
    NeuralNetwork *nn;
    float fitness = 0;
    Ref<NeuralNetworkParameters> neural_network;
};

} // namespace godot

#endif // AGENT_H
