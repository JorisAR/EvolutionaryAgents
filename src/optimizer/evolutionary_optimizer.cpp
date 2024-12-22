#include "evolutionary_optimizer.h"

using namespace godot;

bool EvolutionaryOptimizer::get_verbose() const
{
    return verbose;
}
void EvolutionaryOptimizer::set_verbose(const bool value)
{
    verbose = value;
}

bool EvolutionaryOptimizer::get_log() const
{
    return log;
}
void EvolutionaryOptimizer::set_log(const bool value)
{
    log = value;
}

int EvolutionaryOptimizer::get_debug_generation_interval() const
{
    return debug_generation_interval;
}
void EvolutionaryOptimizer::set_debug_generation_interval(const int value)
{
    debug_generation_interval = value;
}

Ref<EvolutionaryAlgorithmParameters> EvolutionaryOptimizer::get_ea_params() const
{
    return ea_params;
}

void EvolutionaryOptimizer::set_ea_params(Ref<EvolutionaryAlgorithmParameters> value)
{
    ea_params = value;
}

int EvolutionaryOptimizer::get_current_generation() const
{
    return current_generation;
}

void EvolutionaryOptimizer::set_debug_log_path(godot::String value)
{
    debug_log_path = value;
}

godot::String EvolutionaryOptimizer::get_debug_log_path() const
{
    return debug_log_path;
}

bool EvolutionaryOptimizer::get_termination_use_max_generation() const
{
    return termination_use_max_generation;
}
void EvolutionaryOptimizer::set_termination_use_max_generation(bool value)
{
    termination_use_max_generation = value;
}
int EvolutionaryOptimizer::get_termination_max_generation() const
{
    return termination_max_generation;
}
void EvolutionaryOptimizer::set_termination_max_generation(int value)
{
    termination_max_generation = value;
}

void EvolutionaryOptimizer::_bind_methods()
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
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "termination_use_max_generation"), "set_termination_use_max_generation",
                 "get_termination_use_max_generation");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "termination_max_generation"), "set_termination_max_generation",
                 "get_termination_max_generation");
}

EvolutionaryOptimizer::EvolutionaryOptimizer()
{
}
EvolutionaryOptimizer::~EvolutionaryOptimizer()
{
    delete ea;
    delete logger;
}

void EvolutionaryOptimizer::_notification(int p_what)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    if (p_what == NOTIFICATION_EXIT_TREE || p_what == NOTIFICATION_WM_CLOSE_REQUEST || p_what == NOTIFICATION_PREDELETE)
    {
        end_training();
    }
    switch (p_what)
    {
    case NOTIFICATION_READY: {
        call_deferred("start_training");
        break;
    }
    }
}

void EvolutionaryOptimizer::start_training()
{
    if (training)
    {
        UtilityFunctions::printerr("Training is already in process, new call aborted.");
        return;
    }
    training = true;
    if (ea_params == nullptr)
    {
        UtilityFunctions::printerr("No evolutionary algorithm selected, aborted training.");
        return;
    }

    // Initialize the evolutionary strategy
    if (ea == nullptr)
    {
        ea = ea_params->get_evolutionary_algorithm(population_size, genome_size);
    }
    current_generation = 0;

    // Initialize the logger
    if (log)
    {
        if (!debug_log_path.is_empty())
        {
            logger = new Logger(debug_log_path.utf8().get_data());
        }
    }

    best_fitness = -1e9f;
    sum_fitness = 0.0f;
    UtilityFunctions::print("Training started.");

    start_generation();
}

void EvolutionaryOptimizer::start_generation()
{
    current_generation++;
    // Generate population
    fitness_vector = std::vector<float>(population_size, 0.0f);
    generation_best_fitness = -1e9f;
    generation_sum_fitness = 0;
    population = ea->get_population();
}

void godot::EvolutionaryOptimizer::register_fitness(const int i, const float fitness)
{
    sum_fitness += fitness;
    best_fitness = Math::max(best_fitness, fitness);
    generation_sum_fitness += fitness;
    generation_best_fitness = Math::max(generation_best_fitness, fitness);
    fitness_vector[i] = fitness;
}

void EvolutionaryOptimizer::end_generation()
{
    auto generation_mean_fitness = generation_sum_fitness / population_size;

    if (verbose && (current_generation) % debug_generation_interval == 0)
    {
        sum_fitness /= population_size * debug_generation_interval;
        UtilityFunctions::print(static_cast<String>("Generation: " + String::num(current_generation) +
                                                    ", Best Fitness: " + String::num(best_fitness)) +
                                ", Mean Fitness: " + String::num(sum_fitness));

        best_fitness = -1e9f;
        sum_fitness = 0.0f;
    }

    if (log && logger != nullptr)
    {
        logger->log_generation(generation_mean_fitness, generation_best_fitness);
    }

    ea->evolve(fitness_vector);
    emit_signal("generation_ended");

    if (termination_use_max_generation && (current_generation >= termination_max_generation))
    {
        end_training();
    }
    else
        call_deferred("start_generation");
}

void EvolutionaryOptimizer::end_training()
{
    if (!training)
    {
        UtilityFunctions::printerr("Training not in progress, cannot end.");
        return;
    }
    training = false;
    UtilityFunctions::print("Training finished!");

    if (log && logger != nullptr)
    {
        logger->flush();
    }

    delete logger;
    logger = nullptr;

    delete ea;
    ea = nullptr;
}
