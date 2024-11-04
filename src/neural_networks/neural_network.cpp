#include "neural_network.h"
#include <random>
#include <algorithm>
#include <stdexcept>

NeuralNetwork::NeuralNetwork(const std::vector<int>& layers) : layers(layers) {
    if (layers.size() < 2) {
        throw std::invalid_argument("The network must have at least 2 layers (input and output).");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    for (size_t i = 0; i < layers.size() - 1; ++i) {
        int layer_size = layers[i] * layers[i + 1];
        int bias_size = layers[i + 1];
        
        std::vector<float> layer_weights(layer_size);
        std::vector<float> layer_biases(bias_size);

        for (auto& weight : layer_weights) weight = dis(gen);
        for (auto& bias : layer_biases) bias = dis(gen);

        weights.push_back(layer_weights);
        biases.push_back(layer_biases);
    }
}

std::vector<float> NeuralNetwork::infer(const std::vector<float>& inputs) {
    std::vector<float> layer_output = inputs;

    for (size_t i = 0; i < layers.size() - 1; ++i) {
        layer_output = matmul(layer_output, weights[i], layers[i + 1], layer_output.size());
        for (size_t j = 0; j < layer_output.size(); ++j) {
            layer_output[j] += biases[i][j];
        }
        for (auto& val : layer_output) val = activate(val);
    }

    return layer_output;
}

void NeuralNetwork::set_weights_and_biases(const std::vector<float>& new_weights, const std::vector<float>& new_biases) {
    int offset = 0;
    int bias_offset = 0;

    for (size_t i = 0; i < layers.size() - 1; ++i) {
        int layer_size = layers[i] * layers[i + 1];
        int bias_size = layers[i + 1];

        std::copy(new_weights.begin() + offset, new_weights.begin() + offset + layer_size, weights[i].begin());
        offset += layer_size;

        std::copy(new_biases.begin() + bias_offset, new_biases.begin() + bias_offset + bias_size, biases[i].begin());
        bias_offset += bias_size;
    }
}

int NeuralNetwork::get_weights_count() const {
    int count = 0;
    for (const auto& layer_weights : weights) {
        count += layer_weights.size();
    }
    return count;
}

int NeuralNetwork::get_biases_count() const {
    int count = 0;
    for (const auto& layer_biases : biases) {
        count += layer_biases.size();
    }
    return count;
}

int NeuralNetwork::get_input_size() const {
    return layers.front();
}

int NeuralNetwork::get_output_size() const {
    return layers.back();
}

float NeuralNetwork::activate(float x) {
    return std::max(x, 0.0f); // ReLU
}

std::vector<float> NeuralNetwork::matmul(const std::vector<float>& vec, const std::vector<float>& weights, int rows, int cols) {
    std::vector<float> result(rows, 0.0f);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i] += vec[j] * weights[i * cols + j];
        }
    }
    return result;
}
