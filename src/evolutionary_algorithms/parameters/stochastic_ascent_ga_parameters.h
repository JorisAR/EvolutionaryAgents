#ifndef STOCHASTIC_ASCENT_GA_PARAMETERS_H
#define STOCHASTIC_ASCENT_GA_PARAMETERS_H

#include "evolutionary_algorithm_parameters.h"
#include "stochastic_ascent_ga.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
class StochasticAscentGAParameters : public EvolutionaryAlgorithmParameters
{
    GDCLASS(StochasticAscentGAParameters, EvolutionaryAlgorithmParameters)

public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_block_sizes"), &StochasticAscentGAParameters::get_block_sizes);
        ClassDB::bind_method(D_METHOD("set_block_sizes", "block_sizes"), &StochasticAscentGAParameters::set_block_sizes);
        
        ClassDB::bind_method(D_METHOD("get_lower_bound"), &StochasticAscentGAParameters::get_lower_bound);
        ClassDB::bind_method(D_METHOD("set_lower_bound", "lower_bound"), &StochasticAscentGAParameters::set_lower_bound);
        ClassDB::bind_method(D_METHOD("get_upper_bound"), &StochasticAscentGAParameters::get_upper_bound);
        ClassDB::bind_method(D_METHOD("set_upper_bound", "upper_bound"), &StochasticAscentGAParameters::set_upper_bound);
        ClassDB::bind_method(D_METHOD("get_mutation_rate"), &StochasticAscentGAParameters::get_mutation_rate);
        ClassDB::bind_method(D_METHOD("set_mutation_rate", "mutation_rate"), &StochasticAscentGAParameters::set_mutation_rate);
        ClassDB::bind_method(D_METHOD("get_stagnation_count"), &StochasticAscentGAParameters::get_stagnation_count);
        ClassDB::bind_method(D_METHOD("set_stagnation_count", "stagnation_count"), &StochasticAscentGAParameters::set_stagnation_count);

        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lower_bound"), "set_lower_bound", "get_lower_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "upper_bound"), "set_upper_bound", "get_upper_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mutation_rate"), "set_mutation_rate", "get_mutation_rate");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "stagnation_count"), "set_stagnation_count", "get_stagnation_count");
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "block_sizes"), "set_block_sizes","get_block_sizes");
    }

    StochasticAscentGAParameters() {}
    ~StochasticAscentGAParameters() {}

    void _init()
    {
        lower_bound_ = 0.0f;
        upper_bound_ = 1.0f;
        mutation_rate_ = upper_bound_ - lower_bound_;
        stagnation_count_ = 10;
        block_sizes_ = {1};
    }

    float get_lower_bound() const { return lower_bound_; }
    void set_lower_bound(float lower_bound) { lower_bound_ = lower_bound; }

    float get_upper_bound() const { return upper_bound_; }
    void set_upper_bound(float upper_bound) { upper_bound_ = upper_bound; }

    float get_mutation_rate() const { return mutation_rate_; }
    void set_mutation_rate(float mutation_rate) { mutation_rate_ = mutation_rate; }

    int get_stagnation_count() const { return stagnation_count_; }
    void set_stagnation_count(int stagnation_count) { stagnation_count_ = stagnation_count; }

    PackedInt32Array get_block_sizes() const { return block_sizes_; }
    void set_block_sizes(PackedInt32Array block_sizes) { block_sizes_ = block_sizes; }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override
    {
        return new EA::StochasticAscentGA(population_size, individual_size, lower_bound_, upper_bound_, mutation_rate_, stagnation_count_, Utils::array_to_vector_int(block_sizes_));
    }

    

private:
    PackedInt32Array block_sizes_ = {1};
    float lower_bound_ = 0.0f;
    float upper_bound_ = 1.0f;
    float mutation_rate_ = 1.0f;
    int stagnation_count_ = 10;
};
} // namespace godot

#endif
