#include "evolutionary_algorithm.h"

namespace EA
{
EvolutionaryAlgorithm::EvolutionaryAlgorithm(int population_size, int individual_size)
    : population_size(population_size), individual_size(individual_size)
{
    initialize_population();
}

std::vector<std::vector<float>> EvolutionaryAlgorithm::get_population() const
{
    return population;
}

void EvolutionaryAlgorithm::initialize_population()
{
    population.resize(population_size, std::vector<float>(individual_size));
}

void EvolutionaryAlgorithm::set_population(const std::vector<float> individual)
{
    if (individual.size() == individual_size)
    {
        population.assign(population_size, individual);
    }
}
} // namespace EA
