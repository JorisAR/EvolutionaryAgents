#include "problem_optimizer.h"

using namespace godot;

void ProblemOptimizer::start_training()
{
    if (problem == nullptr)
    {
        UtilityFunctions::printerr("No problem selected, aborted training.");
        return;
    }
    if (ea_params == nullptr)
    {
        UtilityFunctions::printerr("No evolutionary algorithm selected, aborted training.");
        return;
    }

    genome_size = problem->get_genome_size();

    // Initialize the evolutionary algorithmalgorithm
    if (ea == nullptr && use_existing_state)
    {
        auto params = EA::EvolutionaryAlgorithmState::from_json(stored_state);
        if (params != nullptr)
        {
            auto genome = params->genome;
            if (genome.size() == genome_size)
            {
                ea = ea_params->get_evolutionary_algorithm(population_size, genome_size);
                ea->set_starting_point(genome);
                UtilityFunctions::print("Initializing population with pre-trained genome");
            }
        }
    }
    
    problem->connect("ended", Callable(this, "on_individual_ended"));
    EvolutionaryOptimizer::start_training();
}

void ProblemOptimizer::start_generation()
{
    EvolutionaryOptimizer::start_generation();
    run_individual();
}

void ProblemOptimizer::run_individual()
{
    auto i = fitness_vector.size();
    problem->emit_signal("started", Utils::vector_to_array_float(population[i]));
}

void ProblemOptimizer::on_individual_ended(const float fitness)
{
    register_fitness(fitness);

    if (fitness_vector.size() >= population_size)
    {
        end_generation();
    }
    else
    {
        call_deferred("run_individual");
    }
}

void ProblemOptimizer::end_generation()
{
    EvolutionaryOptimizer::end_generation();
}

void ProblemOptimizer::end_training()
{
    problem->disconnect("ended", Callable(this, "on_individual_ended"));
    auto genome = ea->get_best_individual();
    // UtilityFunctions::print(static_cast<String>("Final genome: " + Utils::vector_to_string_float(genome)));

    if (save_state)
    {
        auto state = EA::EvolutionaryAlgorithmState();
        state.genome = genome;
        state.save_json(stored_state);
        UtilityFunctions::print("Saved State.");
    }

    EvolutionaryOptimizer::end_training();
}
