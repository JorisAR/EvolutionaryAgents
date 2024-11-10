#ifndef SERIALIZABLE_NEURAL_NETWORK_H
#define SERIALIZABLE_NEURAL_NETWORK_H

#include "utils.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{

class SerializableNeuralNetwork : public Resource
{
    GDCLASS(SerializableNeuralNetwork, Resource)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_parameters"), &SerializableNeuralNetwork::get_parameters);
        ClassDB::bind_method(D_METHOD("set_parameters", "data"), &SerializableNeuralNetwork::set_parameters);
        ClassDB::bind_method(D_METHOD("get_layer_structure"), &SerializableNeuralNetwork::get_layer_structure);
        ClassDB::bind_method(D_METHOD("set_layer_structure", "layers"), &SerializableNeuralNetwork::set_layer_structure);
        ClassDB::bind_method(D_METHOD("is_read_only"), &SerializableNeuralNetwork::is_read_only);
        ClassDB::bind_method(D_METHOD("set_read_only", "read_only"), &SerializableNeuralNetwork::set_read_only);
        // ClassDB::bind_method(D_METHOD("open_log_in_console"), &SerializableNeuralNetwork::open_log_in_console);
        // ClassDB::bind_method(D_METHOD("get_open_log_in_console"), &SerializableNeuralNetwork::get_open_log_in_console);
        // ADD_PROPERTY(PropertyInfo(Variant::BOOL, "open_log_in_console"), "open_log_in_console", "get_open_log_in_console");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "read_only"), "set_read_only", "is_read_only");
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "layer_structure"), "set_layer_structure", "get_layer_structure");
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "parameters"), "set_parameters", "get_parameters");
    }

    PackedFloat32Array get_parameters() const
    {
        return parameters;
    }

    void set_parameters(const PackedFloat32Array &data)
    {
        if (!read_only)
        {
            parameters = data;
        }
    }

    PackedInt32Array get_layer_structure() const
    {
        return layer_structure;
    }

    void set_layer_structure(const PackedInt32Array &layers)
    {
        if (!read_only && layers.size() >= 2)
        {
            layer_structure = layers;
        }
    }

    bool is_read_only() const
    {
        return read_only;
    }

    void set_read_only(bool value)
    {
        read_only = value;
    }

    std::vector<int> get_layers()
    {
        return Utils::array_to_vector_int(layer_structure);
    }

    String get_log_path() {
        auto path = get_path();
        if(path.is_empty()) 
            return path;
        return path.get_basename() + "_log.csv";
    }

    NeuralNetwork *get_neural_network()
    {
        auto layers = Utils::array_to_vector_int(layer_structure);
        auto nn = new NeuralNetwork(layers);
        nn->update(layers, Utils::array_to_vector_float(parameters));

        return nn;
    }

    void open_log_in_console(const bool value)
    {
        String log_path = get_log_path();
        String command = "start cmd.exe /K python plot_fitness.py " + ProjectSettings::get_singleton()->globalize_path(log_path);
        OS::get_singleton()->shell_open("C://Users/name/Downloads");
    }

    bool get_open_log_in_console() const
    {
        return false;
    }

  private:
    PackedFloat32Array parameters;
    PackedInt32Array layer_structure = {1, 1};
    bool read_only = false;
};

} // namespace godot

#endif
