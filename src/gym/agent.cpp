#include "agent.h"

using namespace godot;

PackedFloat32Array godot::Agent::infer(const PackedFloat32Array &state_vector)
{
    if(nn.get_input_size() != state_vector.size()){
        UtilityFunctions::printerr("State vector does not equal neural network input size.");
        return Utils::vector_to_array_float(nn.get_zero_out());
    }

    std::vector<float> outputs = nn.infer(Utils::array_to_vector_float(state_vector));
    auto action_vector = Utils::vector_to_array_float(outputs);
    emit_signal("neural_network_inferred", action_vector);
    return action_vector;
}

void Agent::update(const std::vector<int> &new_layers, const std::vector<float> &parameters)
{    
    nn.update(new_layers, parameters);
}
