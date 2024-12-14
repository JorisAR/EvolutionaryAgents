#ifndef STOCHASTIC_ASCENT_GA_H
#define STOCHASTIC_ASCENT_GA_H

#include "evolutionary_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>
#include <random>

namespace EA
{
class StochasticAscentGA : public EvolutionaryAlgorithm
{
public:
    StochasticAscentGA(int population_size, int individual_size, float lower_bound, float upper_bound, float mutation_rate, int stagnation_count, std::vector<int> block_sizes);
    ~StochasticAscentGA() {}

    void evolve(const std::vector<float> &fitness) override;
    std::vector<float> get_best_individual() override;
    void set_starting_point(const std::vector<float> &individual) override;

private:
    // mutation
    float lower_bound = 0.0f;
    float upper_bound = 1.0f;
    float mutation_rate = 0.5f;
    int stagnation_count = 10; // if not changed for n generations, then divide the mutation rate
    int mutation_count = 0; // times it mutated, return to original;
    std::vector<int> block_sizes; 
    int total_blocks;
    std::vector<std::vector<int>> mutated_blocks_list = std::vector<std::vector<int>>();
    int stagnation_generation_count = 0;

    // Random device and distributions
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis; // we need a gaussian one

    // Best individual found and its fitness
    std::vector<float> best_individual;
    float best_fitness;

    std::vector<std::vector<float>> generate_population();
    void mutate_blocks(std::vector<float> &individual, std::vector<int> &mutated_blocks);
    std::vector<int> get_blocks_to_mutate();
};
} // namespace EA

#endif
