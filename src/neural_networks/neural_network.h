#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

class NeuralNetwork
{
  public:
    NeuralNetwork(const std::vector<int> &layers);

    std::vector<float> infer(const std::vector<float> &inputs);
    std::vector<float> get_zero_out();
    void set_weights_and_biases(const std::vector<float> &weights, const std::vector<float> &biases);
    void update(const std::vector<int> &new_layers, const std::vector<float> parameters);
    int get_input_size() const;
    int get_output_size() const;
    int get_weights_count() const;
    int get_biases_count() const;

    godot::PackedFloat32Array get_parameters() const;

    static std::vector<int> calculate_total_parameters(const std::vector<int> &layers);

  private:
    std::vector<int> layers;
    std::vector<std::vector<float>> weights;
    int total_weight_count;
    std::vector<std::vector<float>> biases;
    int total_bias_count;

    void initialize_weights_and_biases();
    float activate(float x);
    std::vector<float> matmul(const std::vector<float> &vec, const std::vector<float> &weights, int rows, int cols);
};

#endif
