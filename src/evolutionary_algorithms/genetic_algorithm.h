#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "evolutionary_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>
#include <random>

namespace EA
{
class GeneticAlgorithm : public EvolutionaryAlgorithm
{
public:
    enum CrossoverMode {
        ONE_POINT,
        TWO_POINT,
        UNIFORM,
        NONE
    };

    GeneticAlgorithm(int population_size, int individual_size, bool is_binary, float lower_bound, float upper_bound, float mutation_rate, float selection_rate, CrossoverMode crossover_mode);
    ~GeneticAlgorithm() {}

    void evolve(const std::vector<float> &fitness) override;
    std::vector<float> get_best_individual() override;
    void set_starting_point(const std::vector<float> &individual) override;

private:
    CrossoverMode crossover_mode; // Default to ONE_POINT

    // mutation
    bool is_binary;
    float lower_bound;
    float upper_bound;
    float mutation_rate;

    // selection
    float selection_rate;

    // Random device and distributions
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    // Best individual found
    std::vector<float> best_individual;

    std::vector<std::vector<float>> generate_population();
    void crossover(const std::vector<float> &parent1, const std::vector<float> &parent2, std::vector<float> &child);
};
} // namespace EA

#endif
