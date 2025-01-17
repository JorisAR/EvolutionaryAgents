#ifndef AGENT_H
#define AGENT_H

#include "neural_network.h"
#include "neural_network_parameters.h"
#include "utils.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <numeric>
#include <random>
#include <vector>

namespace godot
{

class EvolutionaryAgent : public Node
{
    GDCLASS(EvolutionaryAgent, Node)

  public:
    static void _bind_methods();
    EvolutionaryAgent();
    ~EvolutionaryAgent();

    static int GetMaxElementIndex(const PackedFloat32Array &array);
    static PackedFloat32Array SoftMax(const PackedFloat32Array &array);
    static float SampleNormal(const float mean, const float sigma);
    static int SampleWeightedIndex(const PackedFloat32Array &array, const float random_sample);

    PackedFloat32Array infer(const PackedFloat32Array &state_vector);
    void update(const std::vector<int> &new_layers, const std::vector<float> &parameters);
    void start();

    float get_fitness() const;
    void set_fitness(const float new_fitness);
    Ref<NeuralNetworkParameters> get_neural_network() const;
    void set_neural_network(const Ref<NeuralNetworkParameters> value);

  private:
    NeuralNetwork *nn;
    float fitness = 0;
    Ref<NeuralNetworkParameters> neural_network;
};

} // namespace godot

#endif // AGENT_H
