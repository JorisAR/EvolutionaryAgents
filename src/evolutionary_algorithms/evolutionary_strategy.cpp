#include "evolutionary_strategy.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>

namespace EA
{

EvolutionaryStrategy::EvolutionaryStrategy(int population_size, int individual_size, float sigma, float alpha)
    : EvolutionaryAlgorithm(population_size, individual_size), sigma_(sigma), alpha_(alpha)
{
    auto initial_value = 0.0f;
    if(use_bound)
        initial_value = (lower_bound + upper_bound) / 2.0f;

    weights_ = std::vector<float>(individual_size, initial_value);
    population = generate_population();
}

std::vector<std::vector<float>> EvolutionaryStrategy::generate_population()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0, 1);

    population = std::vector<std::vector<float>>(population_size, std::vector<float>(individual_size));

    for (int i = 0; i < population_size; ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            auto v = weights_[j] + sigma_ * dist(gen);
            if (use_bound)
                v = std::max(lower_bound, std::min(v, upper_bound));
            population[i][j] = v;
        }
    }
    return population;
}

void EvolutionaryStrategy::set_starting_point(const std::vector<float> &individual)
{
    weights_ = individual;
    population = generate_population();
    population[0] = individual;
}

void EvolutionaryStrategy::evolve(const std::vector<float> &fitness)
{
    float mean = std::accumulate(fitness.begin(), fitness.end(), 0.0f) / population_size;
    float variance =
        std::inner_product(fitness.begin(), fitness.end(), fitness.begin(), 0.0f) / population_size - mean * mean;
    float stddev = (variance > 0.0f) ? std::sqrt(variance) : 1.0f;

    for (int i = 0; i < population_size; ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            float update = (population[i][j] - weights_[j]) * (fitness[i] - mean) / stddev;
            weights_[j] += alpha_ / (population_size * sigma_) * update;
        }
    }

    population = generate_population();
}

std::vector<float> EvolutionaryStrategy::get_best_individual()
{
    return weights_;
}
} // namespace EA
