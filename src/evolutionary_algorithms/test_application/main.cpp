#include "neural_network.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>
#include <evolutionary_strategy.h>
#include <sep_cma_es.h>

// Define the dataset for y = x^2
std::vector<std::pair<std::vector<float>, std::vector<float>>> create_dataset() {
    std::vector<std::pair<std::vector<float>, std::vector<float>>> dataset;
    for (float x = -10; x <= 10; x += 0.5) {
        dataset.push_back({{x}, {x * x}});
    }
    return dataset;
}

// Evaluate how well the neural network approximates the function y = x^2
float evaluate_network(NeuralNetwork& nn, const std::vector<std::pair<std::vector<float>, std::vector<float>>>& dataset) {
    float total_error = 0.0f;
    for (const auto& data : dataset) {
        const auto& inputs = data.first;
        const auto& actual_output = data.second;
        auto predicted_output = nn.infer(inputs);
        total_error += std::pow(predicted_output[0] - actual_output[0], 2);
    }
    return std::max(0.0f, total_error / dataset.size());
}

int main() {
    const int population_size = 25;
    const int generations = 10000;
    const double noise_std = 0.5;
    const double learning_rate = 0.01;
    std::vector<int> layers = {1, 10, 1};

    // Create the dataset
    auto dataset = create_dataset();
    NeuralNetwork nn(layers);
    EA::SepCMAES ea(population_size, nn.get_weights_count() + nn.get_biases_count(), noise_std);

    // Open a file to log the fitness values
    std::ofstream log_file("fitness_log.txt");

    std::cout << "Start Training." << std::endl;

    for (int gen = 0; gen < generations; ++gen) {
        auto population = ea.get_population();
        std::vector<float> fitness(population_size);
        for (int i = 0; i < population_size; ++i) {
            std::vector<float> weights(population[i].begin(), population[i].begin() + nn.get_weights_count());
            std::vector<float> biases(population[i].begin() + nn.get_weights_count(), population[i].end());
            nn.set_weights_and_biases(weights, biases);
            fitness[i] = -evaluate_network(nn, dataset); // Negative for minimization
        }
        ea.evolve(fitness);

        // Find the best fitness value and its corresponding weights in the current generation
        auto max_fitness_iter = std::max_element(fitness.begin(), fitness.end());
        int best_individual_index = std::distance(fitness.begin(), max_fitness_iter);
        auto max_fitness = *max_fitness_iter;


        // Log the best fitness value
        if (log_file.is_open()) {
            log_file << -max_fitness << " " << std::endl;
        }

        if (gen % 20 == 19) {
            std::cout << "Generation " << gen << ": Best fitness (mean squared error) = " << -max_fitness << std::endl;
            // std::cout << "Best individual's weights: ";
            // for (const auto& weight : population[best_individual_index]) {
            //     std::cout << weight << " ";
            // }
            // std::cout << std::endl;
        }
    }

    std::cout << "Done." << std::endl;

    auto best = ea.get_best_individual();

    for (size_t i = 0; i < best.size(); i++)
    {
        std::cout << best[i] << ", ";
    }

    std::cout << std::endl;
    

    // Close the log file
    log_file.close();
    return 0;
}
