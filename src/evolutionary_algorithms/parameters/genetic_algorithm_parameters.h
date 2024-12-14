#ifndef GENETIC_ALGORITHM_PARAMETERS_H
#define GENETIC_ALGORITHM_PARAMETERS_H

#include "evolutionary_algorithm_parameters.h"
#include "genetic_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
class GeneticAlgorithmParameters : public EvolutionaryAlgorithmParameters
{
    GDCLASS(GeneticAlgorithmParameters, EvolutionaryAlgorithmParameters)

public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_is_binary"), &GeneticAlgorithmParameters::get_is_binary);
        ClassDB::bind_method(D_METHOD("set_is_binary", "is_binary"), &GeneticAlgorithmParameters::set_is_binary);
        ClassDB::bind_method(D_METHOD("get_lower_bound"), &GeneticAlgorithmParameters::get_lower_bound);
        ClassDB::bind_method(D_METHOD("set_lower_bound", "lower_bound"), &GeneticAlgorithmParameters::set_lower_bound);
        ClassDB::bind_method(D_METHOD("get_upper_bound"), &GeneticAlgorithmParameters::get_upper_bound);
        ClassDB::bind_method(D_METHOD("set_upper_bound", "upper_bound"), &GeneticAlgorithmParameters::set_upper_bound);
        ClassDB::bind_method(D_METHOD("get_mutation_rate"), &GeneticAlgorithmParameters::get_mutation_rate);
        ClassDB::bind_method(D_METHOD("set_mutation_rate", "mutation_rate"), &GeneticAlgorithmParameters::set_mutation_rate);
        ClassDB::bind_method(D_METHOD("get_selection_rate"), &GeneticAlgorithmParameters::get_selection_rate);
        ClassDB::bind_method(D_METHOD("set_selection_rate", "selection_rate"), &GeneticAlgorithmParameters::set_selection_rate);
        ClassDB::bind_method(D_METHOD("get_crossover_mode"), &GeneticAlgorithmParameters::get_crossover_mode);
        ClassDB::bind_method(D_METHOD("set_crossover_mode", "crossover_mode"), &GeneticAlgorithmParameters::set_crossover_mode);

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_binary"), "set_is_binary", "get_is_binary");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lower_bound"), "set_lower_bound", "get_lower_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "upper_bound"), "set_upper_bound", "get_upper_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mutation_rate"), "set_mutation_rate", "get_mutation_rate");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "selection_rate"), "set_selection_rate", "get_selection_rate");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "key", PROPERTY_HINT_ENUM, "OnePoint,TwoPoint,Uniform,None", PROPERTY_USAGE_DEFAULT), "set_crossover_mode", "get_crossover_mode");

        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CrossoverMode::NONE);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CrossoverMode::ONE_POINT);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CrossoverMode::TWO_POINT);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CrossoverMode::UNIFORM);
    }

    GeneticAlgorithmParameters() {}
    ~GeneticAlgorithmParameters() {}

    void _init()
    {
        is_binary_ = false;
        lower_bound_ = 0.0f;
        upper_bound_ = 1.0f;
        mutation_rate_ = 0.05f;
        selection_rate_ = 0.5f;
        crossover_mode_ = EA::GeneticAlgorithm::ONE_POINT;
    }

    bool get_is_binary() const { return is_binary_; }
    void set_is_binary(bool is_binary) { is_binary_ = is_binary; }

    float get_lower_bound() const { return lower_bound_; }
    void set_lower_bound(float lower_bound) { lower_bound_ = lower_bound; }

    float get_upper_bound() const { return upper_bound_; }
    void set_upper_bound(float upper_bound) { upper_bound_ = upper_bound; }

    float get_mutation_rate() const { return mutation_rate_; }
    void set_mutation_rate(float mutation_rate) { mutation_rate_ = mutation_rate; }

    float get_selection_rate() const { return selection_rate_; }
    void set_selection_rate(float selection_rate) { selection_rate_ = selection_rate; }

    int get_crossover_mode() const { return static_cast<int>(crossover_mode_); }
    void set_crossover_mode(int crossover_mode) { crossover_mode_ = static_cast<EA::GeneticAlgorithm::CrossoverMode>(crossover_mode); }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override
    {
        return new EA::GeneticAlgorithm(population_size, individual_size, is_binary_, lower_bound_, upper_bound_, mutation_rate_, selection_rate_, crossover_mode_);
    }

private:
    bool is_binary_ = false;
    float lower_bound_ = 0.0f;
    float upper_bound_ = 1.0f;
    float mutation_rate_ = 0.05f;
    float selection_rate_ = 0.5f;
    EA::GeneticAlgorithm::CrossoverMode crossover_mode_ = EA::GeneticAlgorithm::ONE_POINT;
};
} // namespace godot

VARIANT_ENUM_CAST(EA::GeneticAlgorithm::CrossoverMode);

#endif
