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
#include "json_utils.h"
#include "logger.h"
#include "neural_network_parameters.h"
#include "utils.h"

namespace godot
{

class EvolutionaryOptimizer : public Node
{
    GDCLASS(EvolutionaryOptimizer, Node)

  public:
    static void _bind_methods()
    {
        ADD_SIGNAL(MethodInfo("generation_ended")); // Signal emitted every generation.

        ClassDB::bind_method(D_METHOD("start_training"), &EvolutionaryOptimizer::start_training);
        ClassDB::bind_method(D_METHOD("end_training"), &EvolutionaryOptimizer::end_training);
        ClassDB::bind_method(D_METHOD("start_generation"), &EvolutionaryOptimizer::start_generation);
        ClassDB::bind_method(D_METHOD("end_generation"), &EvolutionaryOptimizer::end_generation);

        ClassDB::bind_method(D_METHOD("set_verbose", "verbose"), &EvolutionaryOptimizer::set_verbose);
        ClassDB::bind_method(D_METHOD("get_verbose"), &EvolutionaryOptimizer::get_verbose);

        ClassDB::bind_method(D_METHOD("set_debug_generation_interval", "interval"),
                             &EvolutionaryOptimizer::set_debug_generation_interval);
        ClassDB::bind_method(D_METHOD("get_debug_generation_interval"),
                             &EvolutionaryOptimizer::get_debug_generation_interval);

        ClassDB::bind_method(D_METHOD("set_log", "log"), &EvolutionaryOptimizer::set_log);
        ClassDB::bind_method(D_METHOD("get_log"), &EvolutionaryOptimizer::get_log);

        ClassDB::bind_method(D_METHOD("set_debug_log_path", "path"), &EvolutionaryOptimizer::set_debug_log_path);
        ClassDB::bind_method(D_METHOD("get_debug_log_path"), &EvolutionaryOptimizer::get_debug_log_path);

        ClassDB::bind_method(D_METHOD("get_current_generation"), &EvolutionaryOptimizer::get_current_generation);

        ClassDB::bind_method(D_METHOD("set_ea_params", "ea_parameters"), &EvolutionaryOptimizer::set_ea_params);
        ClassDB::bind_method(D_METHOD("get_ea_params"), &EvolutionaryOptimizer::get_ea_params);

        ClassDB::bind_method(D_METHOD("get_termination_use_max_generation"),
                             &EvolutionaryOptimizer::get_termination_use_max_generation);
        ClassDB::bind_method(D_METHOD("set_termination_use_max_generation", "value"),
                             &EvolutionaryOptimizer::set_termination_use_max_generation);
        ClassDB::bind_method(D_METHOD("get_termination_max_generation"),
                             &EvolutionaryOptimizer::get_termination_max_generation);
        ClassDB::bind_method(D_METHOD("set_termination_max_generation", "value"),
                             &EvolutionaryOptimizer::set_termination_max_generation);

        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "evolutionary_algorithm_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "EvolutionaryAlgorithmParameters"),
                     "set_ea_params", "get_ea_params");

        ADD_GROUP("Debug", "debug");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_verbose"), "set_verbose", "get_verbose");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "debug_generation_interval"), "set_debug_generation_interval",
                     "get_debug_generation_interval");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_log_csv"), "set_log", "get_log");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "debug_log_path", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR),
                     "set_debug_log_path", "get_debug_log_path");

        ADD_GROUP("Termination", "termination");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "termination_use_max_generation"),
                     "set_termination_use_max_generation", "get_termination_use_max_generation");

        ADD_PROPERTY(PropertyInfo(Variant::INT, "termination_max_generation"), "set_termination_max_generation",
                     "get_termination_max_generation");
    }

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

    int get_current_generation() const
    {
        return current_generation;
    }

    void set_debug_log_path(godot::String value)
    {
        debug_log_path = value;
    }

    godot::String get_debug_log_path() const
    {
        return debug_log_path;
    }

    bool get_termination_use_max_generation() const
    {
        return termination_use_max_generation;
    }
    void set_termination_use_max_generation(bool value)
    {
        termination_use_max_generation = value;
    }
    int get_termination_max_generation() const
    {
        return termination_max_generation;
    }
    void set_termination_max_generation(int value)
    {
        termination_max_generation = value;
    }

  protected:
    int current_generation = 0;
    int genome_size = 1;
    int population_size = 25;
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    EA::EvolutionaryAlgorithm *ea = nullptr;
    bool training = true;

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
