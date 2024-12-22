#ifndef EvolutionaryOptimizer_H
#define EvolutionaryOptimizer_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <mutex>
#include <vector>

#include "evolutionary_algorithm_parameters.h"
#include "logger.h"
#include "neural_network_parameters.h"
#include "utils.h"

namespace godot
{

class EvolutionaryOptimizer : public Node
{
    GDCLASS(EvolutionaryOptimizer, Node)

  public:
    static void _bind_methods();
    void _notification(int what);

    EvolutionaryOptimizer()
    {
    }
    ~EvolutionaryOptimizer()
    {
        delete ea;
        delete logger;
    }

    virtual void start_training();
    virtual void end_training();
    virtual void start_generation();
    virtual void end_generation();
    void register_fitness(const float fitness);

    bool get_verbose() const;
    void set_verbose(const bool value);

    bool get_log() const;
    void set_log(const bool value);

    int get_debug_generation_interval() const;
    void set_debug_generation_interval(const int value);

    Ref<EvolutionaryAlgorithmParameters> get_ea_params() const;
    void set_ea_params(Ref<EvolutionaryAlgorithmParameters> value);

    int get_current_generation() const;
    void set_debug_log_path(godot::String value);

    godot::String get_debug_log_path() const;

    bool get_termination_use_max_generation() const;
    void set_termination_use_max_generation(bool value);
    int get_termination_max_generation() const;
    void set_termination_max_generation(int value);

  protected:
    int current_generation = 0;
    int genome_size = 1;
    int population_size = 25;
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    EA::EvolutionaryAlgorithm *ea = nullptr;
    bool training = false;

    std::vector<float> fitness_vector = std::vector<float>();
    std::vector<std::vector<float>> population = std::vector<std::vector<float>>();
    // termination
    bool termination_use_max_generation = false;
    int termination_max_generation = 1000;

    // logging
    bool verbose = false;
    bool log = false;
    int debug_generation_interval = 100;
    Logger *logger = nullptr;
    godot::String debug_log_path = "res://log.csv";
    float best_fitness = 1e-32f;
    float sum_fitness = 0.0f;
    float generation_best_fitness = 1e-32f;
    float generation_sum_fitness = 0.0f;
};

} // namespace godot

#endif
