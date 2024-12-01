#include "problem_optimizer.h"

using namespace godot;

void ProblemOptimizer::start_training()
{
    if (problem == nullptr)
    {
        UtilityFunctions::printerr("No problem selected, aborted training.");
        return;
    }

    genome_size = problem->get_genome_size();
    problem->connect("ended", Callable(this, "on_individual_ended"));

    EvolutionaryOptimizer::start_training();
}

void ProblemOptimizer::start_generation()
{
    EvolutionaryOptimizer::start_generation();
    run_individual();
}

void ProblemOptimizer::run_individual() {
    auto i = fitness_vector.size();
    problem->emit_signal("started", Utils::vector_to_array_float(population[i]));
}

void ProblemOptimizer::on_individual_ended(const float fitness)
{
    register_fitness(fitness);

    if (fitness_vector.size() >= population_size) {
        end_generation();
    } else {
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
    EvolutionaryOptimizer::end_training();
}
