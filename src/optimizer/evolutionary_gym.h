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
#include "logger.h"
#include "neural_network_parameters.h"
#include "utils.h"
#include "evolutionary_optimizer.h"

namespace godot
{

class EvolutionaryGym : public EvolutionaryOptimizer
{
    GDCLASS(EvolutionaryGym, EvolutionaryOptimizer)

  public:
    static void _bind_methods();

    EvolutionaryGym();

    virtual void start_training() override;
    virtual void end_training() override;
    virtual void start_generation() override;
    virtual void end_generation() override;
    void on_agent_ended();

    void set_agents(const TypedArray<Agent> value);
    TypedArray<Agent> get_agents() const;

    bool get_training() const;
    void set_training(const bool value);

    void set_neural_network(Ref<NeuralNetworkParameters> value);
    Ref<NeuralNetworkParameters> get_neural_network() const;

    int get_current_generation() const;

    int get_physics_ticks_per_second() const;
    void set_physics_ticks_per_second(int value);

    int get_max_fps() const;
    void set_max_fps(int value);

    int get_time_scale() const;
    void set_time_scale(int value);

    void update_engine_settings();

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
