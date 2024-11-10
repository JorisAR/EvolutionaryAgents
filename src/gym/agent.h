#ifndef AGENT_H
#define AGENT_H

#include "neural_network.h"
#include "serializable_neural_network.h"
#include "utils.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

namespace godot
{

class Agent : public Node
{
    GDCLASS(Agent, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("infer"), &Agent::infer);
        ADD_SIGNAL(MethodInfo("neural_network_inferred", PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "action_vector")));

        ClassDB::bind_method(D_METHOD("set_fitness", "value"), &Agent::set_fitness);
        ClassDB::bind_method(D_METHOD("get_fitness"), &Agent::get_fitness);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fitness"), "set_fitness", "get_fitness");

        ClassDB::bind_method(D_METHOD("set_neural_network", "value"), &Agent::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &Agent::get_neural_network);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network", PROPERTY_HINT_RESOURCE_TYPE,
                                  "SerializableNeuralNetwork"),
                     "set_neural_network", "get_neural_network");
    }

    Agent() : nn(NeuralNetwork({1, 1}))
    {
    }
    ~Agent()
    {
    }

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

    void set_neural_network(Ref<SerializableNeuralNetwork> new_neural_network)
    {
        neural_network = new_neural_network;
        if(neural_network == nullptr) return;
            nn.update(neural_network->get_layers(), Utils::array_to_vector_float(neural_network->get_parameters()));
    }

    Ref<SerializableNeuralNetwork> get_neural_network() const
    {
        return neural_network;
    }

  private:
    NeuralNetwork nn;
    float fitness = 0;
    Ref<SerializableNeuralNetwork> neural_network;
};

} // namespace godot

#endif // AGENT_H
