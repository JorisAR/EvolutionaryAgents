#include "evolutionary_optimizer.h"

using namespace godot;

void EvolutionaryOptimizer::start_training()
{
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

    start_generation();
}

void EvolutionaryOptimizer::start_generation()
{
    current_generation++;
    // Generate population
    fitness_vector.clear();
    generation_best_fitness = -1e9f;
    generation_sum_fitness = 0;
    population = ea->get_population();
}


void godot::EvolutionaryOptimizer::register_fitness(const float fitness)
{
    sum_fitness += fitness;
    best_fitness = Math::max(best_fitness, fitness);
    generation_sum_fitness += fitness;
    generation_best_fitness = Math::max(generation_best_fitness, fitness);
    fitness_vector.push_back(fitness);
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

    if(termination_use_max_generation && (current_generation >= termination_max_generation)) {
        end_training();
    } else start_generation();
}

void EvolutionaryOptimizer::end_training()
{
    UtilityFunctions::print("Training finished!");
    auto genome = ea->get_best_individual();
    UtilityFunctions::print(static_cast<String>("Final genome: " + Utils::vector_to_string_float(genome)));

    if (log && logger != nullptr)
    {
        logger->flush();
    }

    delete logger;
    logger = nullptr;

    delete ea;
    ea = nullptr;
}
