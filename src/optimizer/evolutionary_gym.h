#ifndef EvolutionaryGym_H
#define EvolutionaryGym_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <mutex>
#include <vector>

#include "Agent.h"
#include "evolutionary_algorithm_parameters.h"
#include "json_utils.h"
#include "logger.h"
#include "neural_network_parameters.h"
#include "utils.h"

namespace godot
{

class EvolutionaryGym : public Node
{
    GDCLASS(EvolutionaryGym, Node)

  public:
    static void _bind_methods()
    {
        ADD_SIGNAL(MethodInfo("generation_ended")); // Signal emitted every generation.

        ClassDB::bind_method(D_METHOD("start_training"), &EvolutionaryGym::start_training);
        ClassDB::bind_method(D_METHOD("end_training"), &EvolutionaryGym::end_training);
        ClassDB::bind_method(D_METHOD("start_generation"), &EvolutionaryGym::start_generation);
        ClassDB::bind_method(D_METHOD("end_generation"), &EvolutionaryGym::end_generation);

        ClassDB::bind_method(D_METHOD("on_agent_ended"), &EvolutionaryGym::on_agent_ended);
        ClassDB::bind_method(D_METHOD("set_agents", "agents"), &EvolutionaryGym::set_agents);
        ClassDB::bind_method(D_METHOD("get_agents"), &EvolutionaryGym::get_agents);

        ClassDB::bind_method(D_METHOD("set_verbose", "verbose"), &EvolutionaryGym::set_verbose);
        ClassDB::bind_method(D_METHOD("get_verbose"), &EvolutionaryGym::get_verbose);

        ClassDB::bind_method(D_METHOD("set_debug_generation_interval", "interval"),
                             &EvolutionaryGym::set_debug_generation_interval);
        ClassDB::bind_method(D_METHOD("get_debug_generation_interval"),
                             &EvolutionaryGym::get_debug_generation_interval);

        ClassDB::bind_method(D_METHOD("set_log", "log"), &EvolutionaryGym::set_log);
        ClassDB::bind_method(D_METHOD("get_log"), &EvolutionaryGym::get_log);

        ClassDB::bind_method(D_METHOD("get_current_generation"), &EvolutionaryGym::get_current_generation);

        ClassDB::bind_method(D_METHOD("set_ea_params", "ea_parameters"), &EvolutionaryGym::set_ea_params);
        ClassDB::bind_method(D_METHOD("get_ea_params"), &EvolutionaryGym::get_ea_params);

        ClassDB::bind_method(D_METHOD("set_neural_network", "neural network"), &EvolutionaryGym::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &EvolutionaryGym::get_neural_network);

        ClassDB::bind_method(D_METHOD("set_training", "is_training"), &EvolutionaryGym::set_training);
        ClassDB::bind_method(D_METHOD("get_training"), &EvolutionaryGym::get_training);

        ClassDB::bind_method(D_METHOD("get_physics_ticks_per_second"), &EvolutionaryGym::get_physics_ticks_per_second);
        ClassDB::bind_method(D_METHOD("set_physics_ticks_per_second", "value"),
                             &EvolutionaryGym::set_physics_ticks_per_second);

        ClassDB::bind_method(D_METHOD("get_max_fps"), &EvolutionaryGym::get_max_fps);
        ClassDB::bind_method(D_METHOD("set_max_fps", "value"), &EvolutionaryGym::set_max_fps);

        ClassDB::bind_method(D_METHOD("get_time_scale"), &EvolutionaryGym::get_time_scale);
        ClassDB::bind_method(D_METHOD("set_time_scale", "value"), &EvolutionaryGym::set_time_scale);

        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "agents", PROPERTY_HINT_TYPE_STRING,
                                  String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":Agent"),
                     "set_agents", "get_agents");
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "NeuralNetworkParameters"),
                     "set_neural_network", "get_neural_network");
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "evolutionary_algorithm_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "EvolutionaryAlgorithmParameters"),
                     "set_ea_params", "get_ea_params");

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_training"), "set_training", "get_training");

        ADD_GROUP("Time", "");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_ticks_per_second"), "set_physics_ticks_per_second",
                     "get_physics_ticks_per_second");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "max_fps"), "set_max_fps", "get_max_fps");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "time_scale"), "set_time_scale", "get_time_scale");

        ADD_GROUP("Debug", "debug");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_verbose"), "set_verbose", "get_verbose");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_generation_interval"), "set_debug_generation_interval",
                     "get_debug_generation_interval");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_log_csv"), "set_log", "get_log");
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

    bool get_training() const
    {
        return training;
    }
    void set_training(const bool value)
    {
        training = value;
    }

    int get_debug_generation_interval() const
    {
        return debug_generation_interval;
    }
    void set_debug_generation_interval(const int value)
    {
        debug_generation_interval = value;
    }

    Ref<EvolutionaryAlgorithmParameters> get_ea_params() const
    {
        return ea_params;
    }

    void set_ea_params(Ref<EvolutionaryAlgorithmParameters> value)
    {
        ea_params = value;
    }

    void set_neural_network(Ref<NeuralNetworkParameters> value)
    {
        neural_network = value;
    }

    Ref<NeuralNetworkParameters> get_neural_network() const
    {
        return neural_network;
    }

    int get_current_generation() const
    {
        return current_generation;
    }

    int get_physics_ticks_per_second() const
    {
        return physics_ticks_per_second;
    }
    void set_physics_ticks_per_second(int value)
    {
        physics_ticks_per_second = value;
        update_engine_settings();
    }

    int get_max_fps() const
    {
        return max_fps;
    }
    void set_max_fps(int value)
    {
        max_fps = value;
        update_engine_settings();
    }

    int get_time_scale() const
    {
        return time_scale;
    }
    void set_time_scale(int value)
    {
        time_scale = value;
        update_engine_settings();
    }

    void update_engine_settings()
    {
        if (Engine::get_singleton()->is_editor_hint())
        {
            return;
        }

        Engine::get_singleton()->set_max_fps(max_fps);
        Engine::get_singleton()->set_time_scale(time_scale);
        Engine::get_singleton()->set_physics_ticks_per_second(physics_ticks_per_second * time_scale);
        Engine::get_singleton()->set_max_physics_steps_per_frame(
            std::max(1, static_cast<int>(std::ceil(static_cast<float>(physics_ticks_per_second * time_scale) /
                                                   static_cast<float>(max_fps)))));
    }

  private:
    TypedArray<Agent> agents;
    std::vector<Agent *> agent_vector_;
    int current_generation = 0;
    int active_agent_count = 0;
    int agent_count = 0;

    std::vector<int> layers;
    int weight_count = 0;
    int bias_count = 0;
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    EA::EvolutionaryAlgorithm *ea;
    Ref<NeuralNetworkParameters> neural_network;

    Logger *logger;

    bool verbose = false;
    bool log = false;
    bool training = true;
    int debug_generation_interval = 100;
    float best_fitness = 1e-32f;
    float sum_fitness = 0.0f;

    std::mutex agent_ended_mutex;

    int physics_ticks_per_second = 20;
    int max_fps = 30;
    int time_scale = 1;
};

} // namespace godot

#endif
