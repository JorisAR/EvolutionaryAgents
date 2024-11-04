#include "evolutionary_strategy.h"
#include "neural_network.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <fstream>

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
    return total_error / dataset.size();
}

int main() {
    const int population_size = 100;
    const int generations = 10000;
    const double noise_std = 0.1;
    const double learning_rate = 0.01; // Adjusted for better convergence
    std::vector<int> layers = {1, 10, 1}; // Neural network structure: 1 input, 1 hidden layer of 10 neurons, 1 output

    // Create the dataset
    auto dataset = create_dataset();
    NeuralNetwork nn(layers);
    EA::EvolutionaryStrategy es(population_size, nn.get_weights_count() + nn.get_biases_count(), noise_std, learning_rate);

    // Open a file to log the fitness values
    std::ofstream log_file("fitness_log.txt");

    for (int gen = 0; gen < generations; ++gen) {
        auto population = es.get_population();
        std::vector<float> fitness(population_size);
        for (int i = 0; i < population_size; ++i) {
            std::vector<float> weights(population[i].begin(), population[i].begin() + nn.get_weights_count());
            std::vector<float> biases(population[i].begin() + nn.get_weights_count(), population[i].end());
            nn.set_weights_and_biases(weights, biases);
            fitness[i] = -evaluate_network(nn, dataset); // Negative for minimization
        }
        es.evolve(fitness);

        // Log the fitness values
        if (log_file.is_open()) {
            for (const auto& fit : fitness) {
                log_file << -fit << " "; // Log the positive fitness value
            }
            log_file << std::endl;
        }

        // Find the best fitness value and its corresponding weights in the current generation
        auto max_fitness_iter = std::max_element(fitness.begin(), fitness.end());
        int best_individual_index = std::distance(fitness.begin(), max_fitness_iter);
        auto max_fitness = *max_fitness_iter;

        if (gen % 20 == 19) {
            std::cout << "Generation " << gen << ": Best fitness (mean squared error) = " << -max_fitness << std::endl;
            // std::cout << "Best individual's weights: ";
            // for (const auto& weight : population[best_individual_index]) {
            //     std::cout << weight << " ";
            // }
            // std::cout << std::endl;
        }
    }

    // Close the log file
    log_file.close();
    return 0;
}
