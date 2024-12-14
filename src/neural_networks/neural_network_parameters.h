#ifndef NEURAL_NETWORK_PARAMETERS_H
#define NEURAL_NETWORK_PARAMETERS_H

#include "json_utils.h"
#include "neural_network.h"
#include "utils.h"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{

class NeuralNetworkParameters : public Resource
{
    GDCLASS(NeuralNetworkParameters, Resource)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_layer_structure"), &NeuralNetworkParameters::get_layer_structure);
        ClassDB::bind_method(D_METHOD("set_layer_structure", "layers"), &NeuralNetworkParameters::set_layer_structure);

        ClassDB::bind_method(D_METHOD("get_use_existing_network"), &NeuralNetworkParameters::get_use_existing_network);
        ClassDB::bind_method(D_METHOD("set_use_existing_network", "use_existing_network"),
                             &NeuralNetworkParameters::set_use_existing_network);

        ClassDB::bind_method(D_METHOD("get_stored_network"), &NeuralNetworkParameters::get_stored_network);
        ClassDB::bind_method(D_METHOD("set_stored_network", "stored_network"),
                             &NeuralNetworkParameters::set_stored_network);

        ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "layer_structure"), "set_layer_structure",
                     "get_layer_structure");

        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stored_network", PROPERTY_HINT_RESOURCE_TYPE, "JSON"),
                     "set_stored_network", "get_stored_network");

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_existing_network"), "set_use_existing_network",
                     "get_use_existing_network");
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

    std::vector<int> get_layers() const
    {
        return Utils::array_to_vector_int(layer_structure);
    }

    String get_log_path() const
    {
        auto path = get_path();
        if (path.is_empty())
            return path;
        return path.get_basename() + "_log.csv";
    }

    NeuralNetwork *load_neural_network() const
    {
        auto ls = Utils::array_to_vector_int(layer_structure);

        if (!use_existing_network_)
        {
            return new NeuralNetwork(ls);
        }
        else
            return load_neural_network_from_file();
    }

    NeuralNetwork *load_neural_network_from_file() const
    {
        auto ls = Utils::array_to_vector_int(layer_structure);

        if (!stored_network.is_valid())
        {
            UtilityFunctions::printerr("No neural network stored in agent, loading empty network.");
            return new NeuralNetwork(ls);
        }

        auto nn = JSONUtils::deserialize<NeuralNetwork>(stored_network->get_data());

        if (ls != nn->get_layers())
        {
            UtilityFunctions::printerr(
                static_cast<String>("Error loading neural network, layers don't match saved layers."));
            return new NeuralNetwork(ls);
        }

        return nn;
    }

    void save_neural_network_to_file(const std::vector<float> &parameters)
    {
        auto nn = new NeuralNetwork();
        nn->update(Utils::array_to_vector_int(layer_structure), parameters);

        String path = stored_network.is_valid() ? stored_network->get_path() : get_default_network_path();
        if (!path.is_empty())
        {
            JSONUtils::save_to_file<NeuralNetwork>(*nn, path);

            // Update the stored_network resource
            Ref<JSON> json_resource = ResourceLoader::get_singleton()->load(path);
            set_stored_network(json_resource);

            // Ensure the JSON data is correctly reported in the editor
            ResourceSaver::get_singleton()->save(stored_network, path);

            UtilityFunctions::print("Saved neural network to: " + path);
        }
        else
        {
            UtilityFunctions::printerr("Cannot save neural network, the path is empty. (Make sure it's not built-in!)");
        }

        delete nn;
    }

    bool get_use_existing_network() const
    {
        return use_existing_network_;
    }
    void set_use_existing_network(const bool value)
    {
        use_existing_network_ = value;
    }

    Ref<JSON> get_stored_network() const
    {
        return stored_network;
    }
    void set_stored_network(const Ref<JSON> &value)
    {
        stored_network = value;
        notify_property_list_changed();
    }

  private:
    PackedInt32Array layer_structure = {1, 1};
    bool read_only = false;
    Ref<JSON> stored_network;
    bool use_existing_network_ = false;

    String get_default_network_path() const
    {
        auto path = get_path();
        if (path.is_empty())
            return path;
        return path.get_basename() + "_network.json";
    }
};

} // namespace godot

#endif
