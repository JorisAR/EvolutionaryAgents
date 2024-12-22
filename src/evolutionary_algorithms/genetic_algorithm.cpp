#include "genetic_algorithm.h"
#include <algorithm>
#include <godot_cpp/variant/utility_functions.hpp>
#include <numeric>

namespace EA
{

GeneticAlgorithm::GeneticAlgorithm(int population_size, int individual_size, bool is_binary, float lower_bound,
                                   float upper_bound, float mutation_rate, float selection_rate,
                                   CrossoverMode crossover_mode, MutationMode mutation_mode, float mutation_radius)
    : EvolutionaryAlgorithm(population_size, individual_size), is_binary(is_binary), mutation_rate(mutation_rate),
      selection_rate(selection_rate), crossover_mode(crossover_mode), mutation_mode(mutation_mode),
      mutation_radius(mutation_radius), gen(rd()), uniform_dis(0.0f, 1.0f), gaussian_dis(0.0f, mutation_radius)
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
                                 std::vector<float> &child)
{
    std::uniform_int_distribution<int> dist(0, individual_size - 1);

    switch (crossover_mode)
    {
    case CROSSOVER_ONE_POINT: {
        int crossover_point = dist(gen);
        for (int i = 0; i < individual_size; ++i)
        {
            child[i] = i < crossover_point ? parent1[i] : parent2[i];
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
            child[i] = (i < crossover_point1 || i > crossover_point2) ? parent1[i] : parent2[i];
        }
        break;
    }
    case CROSSOVER_UNIFORM: {
        for (int i = 0; i < individual_size; ++i)
        {
            child[i] = uniform_dis(gen) < 0.5 ? parent1[i] : parent2[i];
        }
        break;
    }
    case CROSSOVER_NONE:
    default: {
        child = parent1;
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

void GeneticAlgorithm::evolve(const std::vector<float> &fitness)
{
    std::vector<std::vector<float>> new_population(population_size, std::vector<float>(individual_size));

    // Selection
    std::vector<int> selected_indices(population_size * selection_rate);
    std::iota(selected_indices.begin(), selected_indices.end(), 0);
    std::partial_sort(selected_indices.begin(), selected_indices.end(),
                      selected_indices.begin() + selected_indices.size(),
                      [&fitness](int a, int b) { return fitness[a] > fitness[b]; });
    //evolutation
    for (int i = 0; i < new_population.size(); ++i)
    {
        const auto &parent1 = population[selected_indices[i % selected_indices.size()]];
        const auto &parent2 = population[selected_indices[(i + 1) % selected_indices.size()]];

        std::vector<float> child(individual_size);
        crossover(parent1, parent2, child);
        mutate(child);

        new_population[i] = child;
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
    population[0] = individual;
}

} // namespace EA
