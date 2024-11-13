#include "neural_network.h"
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <algorithm>
#include <random>
#include <stdexcept>

#include "utils.h"

NeuralNetwork::NeuralNetwork(const std::vector<int> &layers) : layers(layers)
{
    initialize_weights_and_biases();
}

void NeuralNetwork::initialize_weights_and_biases()
{
    if (layers.size() < 2)
    {
        throw std::invalid_argument("The network must have at least 2 layers (input and output).");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    weights.clear();
    biases.clear();

    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        int layer_size = layers[i] * layers[i + 1];
        int bias_size = layers[i + 1];

        std::vector<float> layer_weights(layer_size);
        std::vector<float> layer_biases(bias_size);

        for (auto &weight : layer_weights)
            weight = dis(gen);
        for (auto &bias : layer_biases)
            bias = dis(gen);

        weights.push_back(layer_weights);
        biases.push_back(layer_biases);
    }

    total_weight_count = 0;
    for (const auto &layer_weights : weights)
    {
        total_weight_count += layer_weights.size();
    }
    total_bias_count = 0;
    for (const auto &layer_biases : biases)
    {
        total_bias_count += layer_biases.size();
    }
}

std::vector<float> NeuralNetwork::infer(const std::vector<float> &inputs)
{
    std::vector<float> layer_output = inputs;

    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        layer_output = matmul(layer_output, weights[i], layers[i + 1], layer_output.size());
        for (size_t j = 0; j < layer_output.size(); ++j)
        {
            layer_output[j] += biases[i][j];
        }
        for (auto &val : layer_output)
            val = activate(val);
    }

    return layer_output;
}

std::vector<float> NeuralNetwork::get_zero_out()
{
    return std::vector<float>(get_output_size(), 0);
}

void NeuralNetwork::update(const std::vector<int> &new_layers, const std::vector<float> parameters)
{
    if (layers != new_layers)
    {
        layers = new_layers;
        initialize_weights_and_biases();
    }

    if(parameters.size() != total_weight_count + total_bias_count) {
        godot::UtilityFunctions::printerr("Parameters do not match sum of weight and bias counts.");
        return;
    }

    std::vector<float> weights(parameters.begin(), parameters.begin() + total_weight_count);
    std::vector<float> biases(parameters.begin() + total_weight_count, parameters.end());

    set_weights_and_biases(weights, biases);
}

void NeuralNetwork::set_weights_and_biases(const std::vector<float> &new_weights, const std::vector<float> &new_biases)
{
    if (get_weights_count() != new_weights.size() || get_biases_count() != new_biases.size())
    {
        throw std::invalid_argument("The provided weights and biases do not match the network's size.");
    }

    int offset = 0;
    int bias_offset = 0;

    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        int layer_size = layers[i] * layers[i + 1];
        int bias_size = layers[i + 1];

        std::copy(new_weights.begin() + offset, new_weights.begin() + offset + layer_size, weights[i].begin());
        offset += layer_size;

        std::copy(new_biases.begin() + bias_offset, new_biases.begin() + bias_offset + bias_size, biases[i].begin());
        bias_offset += bias_size;
    }
}

int NeuralNetwork::get_weights_count() const
{
    return total_weight_count;
}

int NeuralNetwork::get_biases_count() const
{
    return total_bias_count;
}

godot::PackedFloat32Array NeuralNetwork::get_parameters() const
{
    godot::PackedFloat32Array parameters;
    parameters.resize(total_weight_count + total_bias_count);

    int offset = 0;
    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        std::copy(weights[i].begin(), weights[i].end(), &parameters[offset]);
        offset += weights[i].size();
    }

    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        std::copy(biases[i].begin(), biases[i].end(), &parameters[offset]);
        offset += biases[i].size();
    }

    return parameters;
}


std::vector<int> NeuralNetwork::calculate_total_parameters(const std::vector<int> &layers)
{
    int weights = 0;
    int biases = 0;
    for (size_t i = 0; i < layers.size() - 1; ++i)
    {
        weights += layers[i] * layers[i + 1];
        biases += layers[i + 1];
    }
    return {weights, biases};
}

int NeuralNetwork::get_input_size() const
{
    return layers.front();
}

int NeuralNetwork::get_output_size() const
{
    return layers.back();
}

float NeuralNetwork::activate(float x)
{
    return std::max(x, 0.0f); // ReLU
}

std::vector<float> NeuralNetwork::matmul(const std::vector<float> &vec, const std::vector<float> &weights, int rows,
                                         int cols)
{
    std::vector<float> result(rows, 0.0f);
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            result[i] += vec[j] * weights[i * cols + j];
        }
    }
    return result;
}

godot::Dictionary NeuralNetwork::to_dict() const
{
    godot::Dictionary dict;
    parameters:

    dict["layers"] = godot::Utils::vector_to_array_int(layers);
    dict["parameters"] = get_parameters();
    return dict;
}

void NeuralNetwork::deserialize_from_dict(const godot::Dictionary &dict)
{
    update(godot::Utils::array_to_vector_int(dict["layers"]), 
    godot::Utils::array_to_vector_float(dict["parameters"]));
}
