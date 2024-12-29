#include "genetic_algorithm.h"
#include <algorithm>
#include <godot_cpp/variant/utility_functions.hpp>
#include <numeric>
#include <random>

namespace EA
{

GeneticAlgorithm::GeneticAlgorithm(int population_size, int individual_size, bool is_binary, float lower_bound,
                                   float upper_bound, float mutation_rate, float selection_rate,
                                   CrossoverMode crossover_mode, MutationMode mutation_mode, float mutation_radius, float elitism_rate, int tournament_size)
    : EvolutionaryAlgorithm(population_size, individual_size), is_binary(is_binary), mutation_rate(mutation_rate),
      selection_rate(selection_rate), crossover_mode(crossover_mode), mutation_mode(mutation_mode),
      mutation_radius(mutation_radius), gen(rd()), uniform_dis(0.0f, 1.0f), gaussian_dis(0.0f, mutation_radius), elitism_rate(elitism_rate), tournament_size(tournament_size)
{
    set_bounds(lower_bound, upper_bound);

    population = generate_population();
}

std::vector<std::vector<float>> GeneticAlgorithm::generate_population()
{
    population = std::vector<std::vector<float>>(population_size, std::vector<float>(individual_size));
    std::uniform_real_distribution<float> init_dis(lower_bound, upper_bound);

    for (auto &individual : population)
    {
        for (auto &gene : individual)
        {
            gene = is_binary ? (init_dis(gen) > 0.5 ? upper_bound : lower_bound) : init_dis(gen);
        }
    }
    return population;
}

void GeneticAlgorithm::crossover(const std::vector<float> &parent1, const std::vector<float> &parent2,
                                 std::vector<float> &child1, std::vector<float> &child2)
{
    std::uniform_int_distribution<int> dist(0, individual_size - 1);

    switch (crossover_mode)
    {
    case CROSSOVER_ONE_POINT: {
        int crossover_point = dist(gen);
        for (int i = 0; i < individual_size; ++i)
        {
            child1[i] = i < crossover_point ? parent1[i] : parent2[i];
            child2[i] = i < crossover_point ? parent2[i] : parent1[i];
        }
        break;
    }
    case CROSSOVER_TWO_POINT: {
        int crossover_point1 = dist(gen);
        int crossover_point2 = dist(gen);
        if (crossover_point1 > crossover_point2)
            std::swap(crossover_point1, crossover_point2);
        for (int i = 0; i < individual_size; ++i)
        {
            child1[i] = (i < crossover_point1 || i > crossover_point2) ? parent1[i] : parent2[i];
            child2[i] = (i < crossover_point1 || i > crossover_point2) ? parent2[i] : parent1[i];
        }
        break;
    }
    case CROSSOVER_UNIFORM: {
        for (int i = 0; i < individual_size; ++i)
        {
            child1[i] = uniform_dis(gen) < 0.5 ? parent1[i] : parent2[i];
            child2[i] = uniform_dis(gen) < 0.5 ? parent2[i] : parent1[i];
        }
        break;
    }
    case CROSSOVER_NONE:
    default: {
        child1 = parent1;
        child2 = parent2;
        break;
    }
    }
}

void GeneticAlgorithm::mutate(std::vector<float> &child)
{
    if (mutation_mode == MUTATION_NONE)
        return;

    for (int j = 0; j < individual_size; ++j)
    {
        if (uniform_dis(gen) < mutation_rate)
        {
            if (is_binary)
                child[j] = uniform_dis(gen) > 0.5 ? upper_bound : lower_bound;
            else
            {
                switch (mutation_mode)
                {
                case MUTATION_UNIFORM:
                    child[j] = child[j] + (uniform_dis(gen) - 0.5f) * 2 * mutation_radius;
                    break;
                case MUTATION_GAUSSIAN:
                    child[j] = child[j] + gaussian_dis(gen);
                    break;
                }
                if (use_bound)
                    child[j] = std::max(lower_bound, std::min(upper_bound, child[j]));
            }
        }
    }
}

//tournament with replacement
int GeneticAlgorithm::tournament_selection(const std::vector<float> &fitness, const int selected_size, int tournament_size)
{
    std::uniform_int_distribution<int> dist(0, selected_size - 1);
    int best = dist(gen);
    for (int i = 1; i < tournament_size; ++i)
    {
        int challenger = dist(gen);
        if (fitness[challenger] > fitness[best])
        {
            best = challenger;
        }
    }
    return best;
}

void GeneticAlgorithm::evolve(const std::vector<float> &fitness)
{
    std::vector<std::vector<float>> new_population(population_size, std::vector<float>(individual_size));
    int elite_count = std::ceil(population_size * elitism_rate);
    int selection_count = std::ceil(population_size * selection_rate);

    // Keep the top n% of the population
    std::vector<int> sorted_indices(population_size);
    std::iota(sorted_indices.begin(), sorted_indices.end(), 0);
    std::sort(sorted_indices.begin(), sorted_indices.end(),
              [&fitness](int a, int b) { return fitness[a] > fitness[b]; });
    for (int i = 0; i < elite_count; ++i)
    {
        new_population[i] = population[sorted_indices[i]];
    }

    // Generate the rest using tournament selection, crossover, and mutation
    for (int i = elite_count; i < population_size; i += 2)
    {
        int parent1_index = tournament_selection(fitness, selection_count, tournament_size);
        int parent2_index = tournament_selection(fitness, selection_count, tournament_size);

        const auto &parent1 = population[sorted_indices[parent1_index]];
        const auto &parent2 = population[sorted_indices[parent2_index]];

        std::vector<float> child1(individual_size);
        std::vector<float> child2(individual_size);
        crossover(parent1, parent2, child1, child2);
        mutate(child1);
        mutate(child2);

        new_population[i] = child1;
        if(i + 1 < population_size)
            new_population[i + 1] = child2;
    }

    population = new_population;

    // Update best individual
    auto best_it = std::max_element(fitness.begin(), fitness.end());
    int best_index = std::distance(fitness.begin(), best_it);
    best_individual = population[best_index];
}

std::vector<float> GeneticAlgorithm::get_best_individual()
{
    return best_individual;
}

void GeneticAlgorithm::set_starting_point(const std::vector<float> &individual)
{
    for (auto &ind : population)
    {
        ind = individual;
    }
}


} // namespace EA
