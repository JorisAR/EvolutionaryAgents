#ifndef PROBLEM_OPTIMIZER_H
#define PROBLEM_OPTIMIZER_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

#include "evolutionary_algorithm_parameters.h"
#include "json_utils.h"
#include "utils.h"
#include "evolutionary_optimizer.h"
#include "optimizable_problem.h"

namespace godot
{

class ProblemOptimizer : public EvolutionaryOptimizer
{
    GDCLASS(ProblemOptimizer, EvolutionaryOptimizer)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_problem", "problem"), &ProblemOptimizer::set_problem);
        ClassDB::bind_method(D_METHOD("get_problem"), &ProblemOptimizer::get_problem);
        ClassDB::bind_method(D_METHOD("set_population_size", "population_size"), &ProblemOptimizer::set_population_size);
        ClassDB::bind_method(D_METHOD("get_population_size"), &ProblemOptimizer::get_population_size);
        ClassDB::bind_method(D_METHOD("on_individual_ended", "fitness"), &ProblemOptimizer::on_individual_ended);
        ClassDB::bind_method(D_METHOD("run_individual"), &ProblemOptimizer::run_individual);

        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "problem", PROPERTY_HINT_NODE_TYPE, "OptimizableProblem"),
                     "set_problem", "get_problem");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "population_size"), "set_population_size", "get_population_size"); 
    }

    ProblemOptimizer() : EvolutionaryOptimizer()
    {

    }

    virtual void start_training() override;
    virtual void end_training() override;
    virtual void start_generation() override;
    virtual void end_generation() override;

    void run_individual();
    void on_individual_ended(const float fitness);

    OptimizableProblem* get_problem() const
    {
        return problem;
    }

    void set_problem(OptimizableProblem* value)
    {
        problem = value;
    }

    int get_population_size() const
    {
        return population_size;
    }

    void set_population_size(int value)
    {
        population_size = std::max(1, value);
    }

  private:
    OptimizableProblem *problem = nullptr;
};

} // namespace godot

#endif
