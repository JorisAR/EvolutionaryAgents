#ifndef PROBLEM_OPTIMIZER_H
#define PROBLEM_OPTIMIZER_H

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/json.hpp>
#include <vector>

#include "evolutionary_algorithm_parameters.h"
#include "evolutionary_optimizer.h"
#include "optimizable_problem.h"
#include "utils.h"

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
        ClassDB::bind_method(D_METHOD("set_population_size", "population_size"),
                             &ProblemOptimizer::set_population_size);
        ClassDB::bind_method(D_METHOD("get_population_size"), &ProblemOptimizer::get_population_size);
        ClassDB::bind_method(D_METHOD("on_individual_ended", "fitness"), &ProblemOptimizer::on_individual_ended);
        ClassDB::bind_method(D_METHOD("run_individual"), &ProblemOptimizer::run_individual);

        ClassDB::bind_method(D_METHOD("set_stored_state", "stored_state"), &ProblemOptimizer::set_stored_state);
        ClassDB::bind_method(D_METHOD("get_stored_state"), &ProblemOptimizer::get_stored_state);
        ClassDB::bind_method(D_METHOD("set_use_existing_state", "use_existing_state"), &ProblemOptimizer::set_use_existing_state);
        ClassDB::bind_method(D_METHOD("get_use_existing_state"), &ProblemOptimizer::get_use_existing_state);
        ClassDB::bind_method(D_METHOD("set_save_state", "save_state"), &ProblemOptimizer::set_save_state);
        ClassDB::bind_method(D_METHOD("get_save_state"), &ProblemOptimizer::get_save_state);

        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "problem", PROPERTY_HINT_NODE_TYPE, "OptimizableProblem"),
                     "set_problem", "get_problem");
        ADD_PROPERTY(PropertyInfo(Variant::INT, "population_size"), "set_population_size", "get_population_size");

        ADD_GROUP("Stored State", "stored_");
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stored_state", PROPERTY_HINT_RESOURCE_TYPE, "JSON"),
                     "set_stored_state", "get_stored_state");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stored_use_existing"), "set_use_existing_state",
                     "get_use_existing_state");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stored_save_state"), "set_save_state", "get_save_state");
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

    OptimizableProblem *get_problem() const
    {
        return problem;
    }

    void set_problem(OptimizableProblem *value)
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

    Ref<JSON> get_stored_state() const
    {
        return stored_state;
    }

    void set_stored_state(const Ref<JSON> &value)
    {
        stored_state = value;
        notify_property_list_changed();
    }

    bool get_use_existing_state() const
    {
        return use_existing_state;
    }

    void set_use_existing_state(const bool value)
    {
        use_existing_state = value;
    }

    bool get_save_state() const
    {
        return save_state;
    }

    void set_save_state(const bool value)
    {
        save_state = value;
    }

  private:
    Ref<JSON> stored_state;
    bool use_existing_state = false;
    bool save_state = false;
    OptimizableProblem *problem = nullptr;
};

} // namespace godot

#endif
