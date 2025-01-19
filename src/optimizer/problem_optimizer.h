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
    static void _bind_methods();

    ProblemOptimizer();

    virtual void start_training() override;
    virtual void end_training() override;
    virtual void _start_generation() override;
    virtual void _end_generation() override;

    void run_individual(const int i);
    void on_individual_ended(const float fitness);

    OptimizableProblem *get_problem() const;
    void set_problem(OptimizableProblem *value);

    int get_population_size() const;
    void set_population_size(int value);

    Ref<JSON> get_stored_state() const;
    void set_stored_state(const Ref<JSON> &value);

    bool get_use_existing_state() const;
    void set_use_existing_state(const bool value);

    bool get_save_state() const;
    void set_save_state(const bool value);

    bool get_multithreading() const;
    void set_multithreading(bool value);

  private:
    Ref<JSON> stored_state;
    bool use_existing_state = false;
    bool save_state = false;
    OptimizableProblem *problem = nullptr;
    bool multithreading = false;
    std::atomic<int> active_threads = 0;
    std::atomic<int> handled_problems = 0;
};

} // namespace godot

#endif
