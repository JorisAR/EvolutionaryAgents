#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>

class NeuralNetwork {
public:
    NeuralNetwork(const std::vector<int>& layers);

    std::vector<float> infer(const std::vector<float>& inputs);
    void set_weights_and_biases(const std::vector<float>& weights, const std::vector<float>& biases);
    int get_weights_count() const;
    int get_biases_count() const;
    int get_input_size() const;
    int get_output_size() const;

private:
    std::vector<int> layers;
    std::vector<std::vector<float>> weights;
    std::vector<std::vector<float>> biases;

    float activate(float x);
    std::vector<float> matmul(const std::vector<float>& vec, const std::vector<float>& weights, int rows, int cols);
};

#endif
