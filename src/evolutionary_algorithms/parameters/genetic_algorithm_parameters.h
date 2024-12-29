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

        // mutation
        ClassDB::bind_method(D_METHOD("get_mutation_rate"), &GeneticAlgorithmParameters::get_mutation_rate);
        ClassDB::bind_method(D_METHOD("set_mutation_rate", "mutation_rate"),
                             &GeneticAlgorithmParameters::set_mutation_rate);
        ClassDB::bind_method(D_METHOD("get_mutation_radius"), &GeneticAlgorithmParameters::get_mutation_radius);
        ClassDB::bind_method(D_METHOD("set_mutation_radius", "mutation_radius"),
                             &GeneticAlgorithmParameters::set_mutation_radius);
        ClassDB::bind_method(D_METHOD("get_mutation_mode"), &GeneticAlgorithmParameters::get_mutation_mode);
        ClassDB::bind_method(D_METHOD("set_mutation_mode", "mutation_mode"),
                             &GeneticAlgorithmParameters::set_mutation_mode);

        // selection
        ClassDB::bind_method(D_METHOD("get_selection_rate"), &GeneticAlgorithmParameters::get_selection_rate);
        ClassDB::bind_method(D_METHOD("set_selection_rate", "selection_rate"),
                             &GeneticAlgorithmParameters::set_selection_rate);
        ClassDB::bind_method(D_METHOD("get_elitism_rate"), &GeneticAlgorithmParameters::get_elitism_rate);
        ClassDB::bind_method(D_METHOD("set_elitism_rate", "elitism_rate"),
                             &GeneticAlgorithmParameters::set_elitism_rate);
        ClassDB::bind_method(D_METHOD("get_tournament_size"), &GeneticAlgorithmParameters::get_tournament_size);
        ClassDB::bind_method(D_METHOD("set_tournament_size", "tournament_size"),
                             &GeneticAlgorithmParameters::set_tournament_size);
        // crossover
        ClassDB::bind_method(D_METHOD("get_crossover_mode"), &GeneticAlgorithmParameters::get_crossover_mode);
        ClassDB::bind_method(D_METHOD("set_crossover_mode", "crossover_mode"),
                             &GeneticAlgorithmParameters::set_crossover_mode);

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_binary"), "set_is_binary", "get_is_binary");

        ADD_GROUP("Mutation", "mutation_");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "mutation_mode", PROPERTY_HINT_ENUM, "Uniform,Gaussian,None",
                                  PROPERTY_USAGE_DEFAULT),
                     "set_mutation_mode", "get_mutation_mode");
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::MUTATION_UNIFORM);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::MUTATION_GAUSSIAN);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::MUTATION_NONE);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mutation_rate"), "set_mutation_rate", "get_mutation_rate");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "mutation_radius"), "set_mutation_radius", "get_mutation_radius");

        ADD_GROUP("Selection", "selection_");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "selection_rate"), "set_selection_rate", "get_selection_rate");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "selection_elitism_rate"), "set_elitism_rate", "get_elitism_rate");

        ADD_PROPERTY(PropertyInfo(Variant::INT, "selection_tournament_size"), "set_tournament_size",
                     "get_tournament_size");

        ADD_GROUP("Crossover", "crossover_");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "crossover_mode", PROPERTY_HINT_ENUM, "OnePoint,TwoPoint,Uniform,None",
                                  PROPERTY_USAGE_DEFAULT),
                     "set_crossover_mode", "get_crossover_mode");

        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CROSSOVER_ONE_POINT);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CROSSOVER_TWO_POINT);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CROSSOVER_UNIFORM);
        BIND_ENUM_CONSTANT(EA::GeneticAlgorithm::CROSSOVER_NONE);
    }

    GeneticAlgorithmParameters()
    {
    }
    ~GeneticAlgorithmParameters()
    {
    }

    void _init()
    {
        is_binary = false;

        mutation_rate = 0.05f;
        mutation_radius = 0.5f;
        selection_rate = 0.5f;
        crossover_mode = EA::GeneticAlgorithm::CROSSOVER_ONE_POINT;
        mutation_mode = EA::GeneticAlgorithm::MUTATION_UNIFORM;
    }

    bool get_is_binary() const
    {
        return is_binary;
    }
    void set_is_binary(bool value)
    {
        is_binary = value;
    }

    float get_selection_rate() const
    {
        return selection_rate;
    }
    void set_selection_rate(float value)
    {
        selection_rate = value;
    }

    int get_crossover_mode() const
    {
        return static_cast<int>(crossover_mode);
    }
    void set_crossover_mode(int value)
    {
        this->crossover_mode = static_cast<EA::GeneticAlgorithm::CrossoverMode>(value);
    }

    int get_mutation_mode() const
    {
        return static_cast<int>(mutation_mode);
    }
    void set_mutation_mode(int value)
    {
        this->mutation_mode = static_cast<EA::GeneticAlgorithm::MutationMode>(value);
    }

    float get_mutation_rate() const
    {
        return mutation_rate;
    }
    void set_mutation_rate(float value)
    {
        mutation_rate = value;
    }

    float get_mutation_radius() const
    {
        return mutation_radius;
    }
    void set_mutation_radius(float value)
    {
        mutation_radius = value;
    }

    float get_elitism_rate() const
    {
        return elitism_rate;
    }
    void set_elitism_rate(float value)
    {
        elitism_rate = value;
    }

    int get_tournament_size() const
    {
        return tournament_size;
    }
    void set_tournament_size(int value)
    {
        tournament_size = value;
    }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override
    {
        auto ea = new EA::GeneticAlgorithm(population_size, individual_size, is_binary, get_lower_bound(),
                                           get_upper_bound(), mutation_rate, selection_rate, crossover_mode,
                                           mutation_mode, mutation_radius, elitism_rate, tournament_size);
        if (get_use_bound())
            ea->set_bounds(get_lower_bound(), get_upper_bound());
        return ea;
    }

  private:
    bool is_binary = false;

    float mutation_rate = 0.05f;
    float mutation_radius = 0.5f;
    float selection_rate = 0.5f;
    float elitism_rate = 0.05f;
    int tournament_size = 4;
    EA::GeneticAlgorithm::CrossoverMode crossover_mode = EA::GeneticAlgorithm::CROSSOVER_ONE_POINT;
    EA::GeneticAlgorithm::MutationMode mutation_mode = EA::GeneticAlgorithm::MUTATION_UNIFORM;
};
} // namespace godot

VARIANT_ENUM_CAST(EA::GeneticAlgorithm::CrossoverMode);
VARIANT_ENUM_CAST(EA::GeneticAlgorithm::MutationMode);

#endif
