#include "firefly_algorithm.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

namespace EA {

FireflyAlgorithm::FireflyAlgorithm(int population_size, int individual_size, float beta, float gamma, float alpha)
    : EvolutionaryAlgorithm(population_size, individual_size), beta_(beta), gamma_(gamma), alpha_(alpha) {
    generate_population();
}

std::vector<std::vector<float>> FireflyAlgorithm::generate_population() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0, 1);

    std::vector<std::vector<float>> population(population_size, std::vector<float>(individual_size));
    for (int i = 0; i < population_size; ++i) {
        for (int j = 0; j < individual_size; ++j) {
            population[i][j] = dist(gen);
        }
    }
    return population;
}

void FireflyAlgorithm::update_firefly(std::vector<float>& firefly_i, const std::vector<float>& firefly_j, float attractiveness) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0, 1);

    for (int d = 0; d < individual_size; ++d) {
        firefly_i[d] += beta_ * std::exp(-gamma_ * std::pow(firefly_i[d] - firefly_j[d], 2)) * (firefly_j[d] - firefly_i[d])
                        + alpha_ * dist(gen);
    }
}

void FireflyAlgorithm::evolve(const std::vector<float>& fitness) {
    for (int i = 0; i < population_size; ++i) {
        for (int j = 0; j < population_size; ++j) {
            if (fitness[j] > fitness[i]) {
                update_firefly(population[i], population[j], beta_);
            }
        }
    }
}

std::vector<float> FireflyAlgorithm::get_best_individual() {
    return *std::max_element(population.begin(), population.end(), [](const std::vector<float>& a, const std::vector<float>& b) {
        return std::accumulate(a.begin(), a.end(), 0.0f) < std::accumulate(b.begin(), b.end(), 0.0f);
    });
}

} // namespace EA
