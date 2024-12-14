#include "stochastic_ascent_ga.h"
#include <algorithm>
#include <numeric>

namespace EA
{

StochasticAscentGA::StochasticAscentGA(int population_size, int individual_size, float lower_bound, float upper_bound,
                                       float mutation_rate, int stagnation_count, std::vector<int> block_sizes)
    : EvolutionaryAlgorithm(population_size, individual_size), lower_bound(lower_bound), upper_bound(upper_bound),
      mutation_rate(mutation_rate), stagnation_count(stagnation_count), block_sizes(block_sizes), gen(rd()),
      dis(lower_bound, upper_bound)
{
    // Initialize best individual randomly
    std::uniform_real_distribution<float> init_dis(lower_bound, upper_bound);
    best_individual.resize(individual_size);
    for (auto &gene : best_individual)
    {
        gene = init_dis(gen);
    }
    best_fitness = -1e6f; // Initialize best fitness to a small number

    int sum_block_sizes = std::accumulate(block_sizes.begin(), block_sizes.end(), 0);
    total_blocks = individual_size / sum_block_sizes;

    population = generate_population();
}

std::vector<std::vector<float>> StochasticAscentGA::generate_population()
{
    std::vector<std::vector<float>> population(population_size, best_individual);
    mutated_blocks_list.resize(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        mutate_blocks(population[i], mutated_blocks_list[i]);
    }
    return population;
}

void StochasticAscentGA::mutate_blocks(std::vector<float> &individual, std::vector<int> &mutated_blocks)
{
    std::uniform_int_distribution<int> block_count_dis(1, (total_blocks + population_size - 1) / population_size);
    int M = 1;//block_count_dis(gen);
    std::uniform_int_distribution<int> block_index_dis(0, total_blocks - 1);
    mutated_blocks.clear();
    for (int i = 0; i < M; ++i)
    {
        int block_index;
        do
        {
            block_index = block_index_dis(gen);
        } while (std::find(mutated_blocks.begin(), mutated_blocks.end(), block_index) != mutated_blocks.end());
        mutated_blocks.push_back(block_index);
        int sum_block_sizes = std::accumulate(block_sizes.begin(), block_sizes.end(), 0);
        int relative_index = block_index % block_sizes.size();
        int start_index = block_index / block_sizes.size() * sum_block_sizes +
                          std::accumulate(block_sizes.begin(), block_sizes.begin() + relative_index, 0);
        std::normal_distribution<float> gauss(0.0f, mutation_rate);
        for (int j = 0; j < block_sizes[relative_index]; ++j)
        {
            int index = start_index + j;
            individual[index] = std::clamp(individual[index] + gauss(gen), lower_bound, upper_bound);
        }
    }
}

void StochasticAscentGA::evolve(const std::vector<float> &fitness)
{
    bool changed = false;
    for (int i = 0; i < population_size; ++i)
    {
        if (fitness[i] > best_fitness)
        {
            for (int block_index : mutated_blocks_list[i])
            {
                int sum_block_sizes = std::accumulate(block_sizes.begin(), block_sizes.end(), 0);
                int relative_index = block_index % block_sizes.size();
                int start_index = block_index / block_sizes.size() * sum_block_sizes +
                                  std::accumulate(block_sizes.begin(), block_sizes.begin() + relative_index, 0);

                for (int j = 0; j < block_sizes[relative_index]; ++j)
                {
                    int index = start_index + j;
                    best_individual[index] = population[i][index];
                }
            }
            best_fitness = fitness[i];
            changed = true;
        }
    }

    if (changed)
    {
        ++stagnation_generation_count;
    }
    else
    {
        stagnation_generation_count = 0;
    }

    if (stagnation_generation_count >= stagnation_count)
    {
        mutation_rate *= 0.5f;
        stagnation_generation_count = 0;
    }

    population = generate_population();
}

std::vector<float> StochasticAscentGA::get_best_individual()
{
    return best_individual;
}

void StochasticAscentGA::set_starting_point(const std::vector<float> &individual)
{
    best_individual = individual;
    best_fitness = -1e6f; // Reset best fitness
}

} // namespace EA
