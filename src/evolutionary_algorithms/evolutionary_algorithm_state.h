#ifndef EVOLUTIONARY_ALGORITHM_STATEH
#define EVOLUTIONARY_ALGORITHM_STATEH

#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <vector>

#include "neural_network.h"
#include "utils.h"

namespace EA
{
class EvolutionaryAlgorithmState : public ISerializable<EvolutionaryAlgorithmState>
{
  public:
    std::vector<float> genome = {};
    std::vector<int> layers = {};
    int generation = 0;

    godot::Dictionary to_dict() const override
    {
        godot::Dictionary dict;

        if (genome.size() > 0)
        {
            dict["layers"] = godot::Utils::vector_to_array_int(layers);
        }

        if (genome.size() > 0)
        {
            dict["genome"] = godot::Utils::vector_to_array_float(genome);
        }
        dict["generation"] = generation;
        return dict;
    }

    void deserialize_from_dict(const godot::Dictionary &dict) override
    {
        if (dict.has("layers"))
        {
            layers = godot::Utils::array_to_vector_int(dict["layers"]);
        }

        if (dict.has("genome"))
        {
            genome = godot::Utils::array_to_vector_float(dict["genome"]);
        }

        if (dict.has("generation"))
        {
            generation = dict["generation"];
        }
    }

    static EvolutionaryAlgorithmState *from_json(const godot::Ref<godot::JSON> json) {
        if(json.is_null()) {
            godot::UtilityFunctions::printerr("Json is not set, cannot load!");
            return nullptr;
        }
        auto state = new EvolutionaryAlgorithmState();
        state->deserialize_from_dict(json->get_data());
        return state;
    }

    void save_json(const godot::Ref<godot::JSON> json) {
        if(json.is_null()) {
            godot::UtilityFunctions::printerr("Json is not set, cannot save!");
            return;
        }
        if(json->get_path().is_empty()) {
            godot::UtilityFunctions::printerr("Json has no path, cannot save!");
            return;
        }
        json->set_data(to_dict());
        godot::ResourceSaver::get_singleton()->save(json, json->get_path());
    }
};
} // namespace EA

#endif
