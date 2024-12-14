#ifndef GYM_H
#define GYM_H

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
#include "evolutionary_optimizer.h"

namespace godot
{

class Gym : public EvolutionaryOptimizer
{
    GDCLASS(Gym, EvolutionaryOptimizer)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("on_agent_ended"), &Gym::on_agent_ended);
        ClassDB::bind_method(D_METHOD("set_agents", "agents"), &Gym::set_agents);
        ClassDB::bind_method(D_METHOD("get_agents"), &Gym::get_agents);

        ClassDB::bind_method(D_METHOD("set_neural_network", "neural network"), &Gym::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &Gym::get_neural_network);

        ClassDB::bind_method(D_METHOD("get_physics_ticks_per_second"), &Gym::get_physics_ticks_per_second);
        ClassDB::bind_method(D_METHOD("set_physics_ticks_per_second", "value"),
                             &Gym::set_physics_ticks_per_second);

        ClassDB::bind_method(D_METHOD("get_max_fps"), &Gym::get_max_fps);
        ClassDB::bind_method(D_METHOD("set_max_fps", "value"), &Gym::set_max_fps);

        ClassDB::bind_method(D_METHOD("get_time_scale"), &Gym::get_time_scale);
        ClassDB::bind_method(D_METHOD("set_time_scale", "value"), &Gym::set_time_scale);

        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "agents", PROPERTY_HINT_TYPE_STRING,
                                  String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":Agent"),
                     "set_agents", "get_agents");
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "NeuralNetworkParameters"),
                     "set_neural_network", "get_neural_network");


        ADD_GROUP("Time", "");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_ticks_per_second"), "set_physics_ticks_per_second",
                     "get_physics_ticks_per_second");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "max_fps"), "set_max_fps", "get_max_fps");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "time_scale"), "set_time_scale", "get_time_scale");
    }

    Gym() : EvolutionaryOptimizer()
    {
    }

    virtual void start_training() override;
    virtual void end_training() override;
    virtual void start_generation() override;
    virtual void end_generation() override;
    void on_agent_ended();


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

  

    bool get_training() const
    {
        return training;
    }
    void set_training(const bool value)
    {
        training = value;
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
    int active_agent_count = 0;
    int agent_count = 0;

    std::vector<int> layers;
    Ref<NeuralNetworkParameters> neural_network;

    std::mutex agent_ended_mutex;

    int physics_ticks_per_second = 20;
    int max_fps = 30;
    int time_scale = 1;
};

} // namespace godot

#endif
