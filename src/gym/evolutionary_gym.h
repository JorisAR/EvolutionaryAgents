#ifndef EvolutionaryGym_H
#define EvolutionaryGym_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

#include "Agent.h"
#include "evolutionary_algorithm_parameters.h"
#include "evolutionary_strategy.h"
#include "logger.h"
#include "serializable_neural_network.h"
#include "utils.h"

namespace godot
{

class EvolutionaryGym : public Node
{
    GDCLASS(EvolutionaryGym, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("start_training"), &EvolutionaryGym::start_training);
        ClassDB::bind_method(D_METHOD("end_training"), &EvolutionaryGym::end_training);
        ClassDB::bind_method(D_METHOD("start_generation"), &EvolutionaryGym::start_generation);
        ClassDB::bind_method(D_METHOD("end_generation"), &EvolutionaryGym::end_generation);

        ClassDB::bind_method(D_METHOD("on_agent_ended"), &EvolutionaryGym::on_agent_ended);
        ClassDB::bind_method(D_METHOD("set_agents", "agents"), &EvolutionaryGym::set_agents);
        ClassDB::bind_method(D_METHOD("get_agents"), &EvolutionaryGym::get_agents);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "agents", PROPERTY_HINT_TYPE_STRING,
                                  String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":Agent"),
                     "set_agents", "get_agents"); // The list of agents the gym will use during training.

        ClassDB::bind_method(D_METHOD("set_verbose", "verbose"), &EvolutionaryGym::set_verbose);
        ClassDB::bind_method(D_METHOD("get_verbose"), &EvolutionaryGym::get_verbose);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "verbose"), "set_verbose", "get_verbose");

        ClassDB::bind_method(D_METHOD("set_log", "log"), &EvolutionaryGym::set_log);
        ClassDB::bind_method(D_METHOD("get_log"), &EvolutionaryGym::get_log);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "log"), "set_log", "get_log");

        ClassDB::bind_method(D_METHOD("get_current_generation"), &EvolutionaryGym::get_current_generation);
        ADD_SIGNAL(MethodInfo("generation_ended")); // Signal emitted every generation.

        ClassDB::bind_method(D_METHOD("set_ea_params", "ea_parameters"), &EvolutionaryGym::set_ea_params);
        ClassDB::bind_method(D_METHOD("get_ea_params"), &EvolutionaryGym::get_ea_params);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "evolutionary_algorithm_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "EvolutionaryAlgorithmParameters"),
                     "set_ea_params", "get_ea_params"); // The evolutionary algorithm the gym will use as an optimizer.

        ClassDB::bind_method(D_METHOD("set_neural_network", "neural network"), &EvolutionaryGym::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &EvolutionaryGym::get_neural_network);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "serializable_neural_network", PROPERTY_HINT_RESOURCE_TYPE,
                                  "SerializableNeuralNetwork"),
                     "set_neural_network", "get_neural_network"); // The neural network the gym will train. The gym will
                                                                  // overwrite the trained parameters when saved.
    }

    EvolutionaryGym() : ea(nullptr)
    {
    }
    ~EvolutionaryGym()
    {
        delete ea;
    }

    void start_training();
    void end_training();
    void start_generation();
    void on_agent_ended();
    void end_generation();

    void set_agents(const TypedArray<Agent> value)
    {
        agents = value;
        agent_vector_.clear();
        for (size_t i = 0; i < agents.size(); i++)
        {
            Agent *agent = Object::cast_to<Agent>(agents[i]);
            if (agent)
            {
                agent_vector_.push_back(agent);
            }
        }
    }
    TypedArray<Agent> get_agents() const
    {
        return agents;
    }

    bool get_verbose() const
    {
        return verbose;
    }
    void set_verbose(const bool value)
    {
        verbose = value;
    }

    bool get_log() const
    {
        return log;
    }
    void set_log(const bool value)
    {
        log = value;
    }

    Ref<EvolutionaryAlgorithmParameters> get_ea_params() const
    {
        return ea_params;
    }

    void set_ea_params(Ref<EvolutionaryAlgorithmParameters> value)
    {
        ea_params = value;
    }

    void set_neural_network(Ref<SerializableNeuralNetwork> value)
    {
        neural_network = value;
    }

    Ref<SerializableNeuralNetwork> get_neural_network() const
    {
        return neural_network;
    }

    int get_current_generation() const
    {
        return current_generation;
    }

  private:
    TypedArray<Agent> agents;
    std::vector<Agent*> agent_vector_;
    int current_generation = 0;
    int active_agent_count = 0;
    int agent_count = 0;

    std::vector<int> layers;
    int weight_count = 0;
    int bias_count = 0;
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    EA::EvolutionaryAlgorithm *ea;
    bool verbose = false;
    bool log = false;

    Logger* logger;
    Ref<SerializableNeuralNetwork> neural_network;
};

} // namespace godot

#endif
