#include "evolutionary_strategy.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

namespace EA {

    EvolutionaryStrategy::EvolutionaryStrategy(int population_size, int individual_size, float sigma, float alpha)
        : EvolutionaryAlgorithm(population_size, individual_size), sigma_(sigma), alpha_(alpha) {
        weights_ = std::vector<float>(individual_size, 0.0f);
        population = generate_population();
    }

    std::vector<std::vector<float>> EvolutionaryStrategy::generate_population() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0, 1);

        population = std::vector<std::vector<float>>(population_size, std::vector<float>(individual_size));

        for (int i = 0; i < population_size; ++i) {
            for (int j = 0; j < individual_size; ++j) {
                population[i][j] = weights_[j] + sigma_ * dist(gen);
            }
        }
    }

    void EvolutionaryStrategy::set_starting_point(const std::vector<float> &individual)
    {
        weights_ = individual;
        population = generate_population();
    }

    void EvolutionaryStrategy::evolve(const std::vector<float>& fitness) {
        float mean = std::accumulate(fitness.begin(), fitness.end(), 0.0f) / population_size;
        float variance = std::inner_product(fitness.begin(), fitness.end(), fitness.begin(), 0.0f) / population_size - mean * mean;
        float stddev = (variance > 0.0f) ? std::sqrt(variance) : 1.0f;

        for (int i = 0; i < population_size; ++i) {
            for (int j = 0; j < individual_size; ++j) {
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
