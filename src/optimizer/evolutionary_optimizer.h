#ifndef EVOLUTIONARY_OPTIMIZER_H
#define EVOLUTIONARY_OPTIMIZER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>

#include "evolutionary_algorithm_parameters.h"
#include "optimizable_problem.h"
#include "utils.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot
{

class EvolutionaryOptimizer : public Node
{
    GDCLASS(EvolutionaryOptimizer, Node)

  public:
    static void _bind_methods()
    {
        // add population count
        ClassDB::bind_method(D_METHOD("set_ea_params", "ea_parameters"), &EvolutionaryOptimizer::set_ea_params);
        ClassDB::bind_method(D_METHOD("get_ea_params"), &EvolutionaryOptimizer::get_ea_params);
        ClassDB::bind_method(D_METHOD("set_problem", "problem"), &EvolutionaryOptimizer::set_problem);
        ClassDB::bind_method(D_METHOD("get_problem"), &EvolutionaryOptimizer::get_problem);

        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "evolutionary_algorithm_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "EvolutionaryAlgorithmParameters"),
                     "set_ea_params", "get_ea_params");
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "problem", PROPERTY_HINT_NODE_TYPE, "OptimizableProblem"),
                     "set_problem", "get_problem");

        ClassDB::bind_method(D_METHOD("start_training"), &EvolutionaryOptimizer::start_training);
    }

    Ref<EvolutionaryAlgorithmParameters> get_ea_params() const
    {
        return ea_params;
    }

    void set_ea_params(Ref<EvolutionaryAlgorithmParameters> value)
    {
        ea_params = value;
    }

    OptimizableProblem* get_problem() const
    {
        return problem;
    }

    void set_problem(OptimizableProblem* value)
    {
        problem = value;
    }


    void start_training()
    {
        if (problem == nullptr || !ea_params.is_valid())
        {
            UtilityFunctions::printerr("Problem or EA parameters are not valid.");
            return;
        }
        ea = ea_params->get_evolutionary_algorithm(population_size, problem->get_parameter_count());

        for (size_t g = 0; g < max_generation_count; g++)
        {
            auto population = ea->get_population();
            for (size_t i = 0; i < population_size; i++)
            {
                PackedFloat32Array gene = Utils::vector_to_array_float(population[i]);
                // auto fitness = problem->compute(gene);
                // fitness_values.push_back(fitness);
            }
            ea->evolve(fitness_values);
            fitness_values.clear();
        }   
    }

  private:
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    OptimizableProblem *problem = nullptr;
    EA::EvolutionaryAlgorithm *ea = nullptr;
    std::vector<float> fitness_values;
    int population_size = 25;
    int max_generation_count = 100;
};

} // namespace godot

#endif // EVOLUTIONARY_OPTIMIZER_H
