#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "evolutionary_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <random>
#include <vector>

namespace EA
{
class GeneticAlgorithm : public EvolutionaryAlgorithm
{
  public:
    enum CrossoverMode
    {
        CROSSOVER_ONE_POINT,
        CROSSOVER_TWO_POINT,
        CROSSOVER_UNIFORM,
        CROSSOVER_NONE
    };
    enum MutationMode
    {
        MUTATION_UNIFORM,
        MUTATION_GAUSSIAN,
        MUTATION_NONE
    };

    GeneticAlgorithm(int population_size, int individual_size, bool is_binary, float lower_bound, float upper_bound,
                     float mutation_rate, float selection_rate, CrossoverMode crossover_mode,
                     MutationMode mutation_mode, float mutation_radius);
    ~GeneticAlgorithm()
    {
    }

    void evolve(const std::vector<float> &fitness) override;
    std::vector<float> get_best_individual() override;
    void set_starting_point(const std::vector<float> &individual) override;

  private:
    bool is_binary;
    CrossoverMode crossover_mode; // Default to ONE_POINT

    // mutation
    MutationMode mutation_mode; // Default to ONE_POINT+
    float mutation_rate;
    float mutation_radius;

    // selection
    float selection_rate;

    // Random device and distributions
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> uniform_dis;
    std::normal_distribution<float> gaussian_dis;

    // Best individual found
    std::vector<float> best_individual;

    std::vector<std::vector<float>> generate_population();
    void crossover(const std::vector<float> &parent1, const std::vector<float> &parent2, std::vector<float> &child);
    void mutate(std::vector<float> &child);
};
} // namespace EA

#endif
