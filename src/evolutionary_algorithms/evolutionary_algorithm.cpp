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

void EvolutionaryAlgorithm::clamp_population()
{
    if (!use_bound) return;

    for (auto &individual : population)
    {
        for (auto &value : individual)
        {
            value = std::max(lower_bound, std::min(value, upper_bound));
        }
    }
}

void EvolutionaryAlgorithm::set_population(const std::vector<float> individual)
{
    if (individual.size() == individual_size)
    {
        population.assign(population_size, individual);
    }
}

void EvolutionaryAlgorithm::set_bounds(const float lower_bound, const float upper_bound)
{
    use_bound = true;
    this->lower_bound = lower_bound;
    this->upper_bound = upper_bound;
}

} // namespace EA
