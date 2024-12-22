#include "problem_optimizer.h"
#include <atomic>
#include <thread>

using namespace godot;

void ProblemOptimizer::start_training()
{
    if(multithreading) {
        multithreading = false;
        UtilityFunctions::printerr("Multithreading not supported for Problem Optimizer at the moment, disabled.");
    }
    if (problem == nullptr)
    {
        UtilityFunctions::printerr("No problem selected, aborted training.");
        return;
    }
    if (ea_params == nullptr)
    {
        UtilityFunctions::printerr("No evolutionary algorithm selected, aborted training.");
        return;
    }

    genome_size = problem->get_genome_size();

    // Initialize the evolutionary algorithm
    if (ea == nullptr && use_existing_state)
    {
        auto params = EA::EvolutionaryAlgorithmState::from_json(stored_state);
        if (params != nullptr)
        {
            auto genome = params->genome;
            if (genome.size() == genome_size)
            {
                ea = ea_params->get_evolutionary_algorithm(population_size, genome_size);
                ea->set_starting_point(genome);
                UtilityFunctions::print("Initializing population with pre-trained genome");
            }
        }
    }

    problem->connect("ended", Callable(this, "on_individual_ended"));
    EvolutionaryOptimizer::start_training();
}

void ProblemOptimizer::start_generation()
{
    EvolutionaryOptimizer::start_generation();
    active_threads = 0;
    handled_problems = 0;
    if (multithreading)
    {
        active_threads = population_size;
        for (int i = 0; i < population_size; ++i)
        {
            std::thread([this]() { run_individual(handled_problems++); }).detach();
        }
    }
    else
    {
        run_individual(handled_problems++);
    }
}

void ProblemOptimizer::run_individual(const int i)
{
    if (i >= population_size)
        return;
    problem->emit_signal("started", Utils::vector_to_array_float(population[i]));
    //float fitness = problem->call("_evaluate", Utils::vector_to_array_float(population[i]));
    //on_individual_ended(i, fitness);
}

void ProblemOptimizer::on_individual_ended(const float fitness) //const int i, 
{
    auto i = handled_problems - 1; //only works for single threading
    register_fitness(i, fitness);

    //UtilityFunctions::print(active_threads.load());
    if ((!multithreading || (--active_threads) <= 0) && handled_problems >= population_size) // await all threads
    {
        call_deferred("end_generation");
    }

    if (handled_problems < population_size && !multithreading)
    {
        run_individual(handled_problems++);
    }
}

void ProblemOptimizer::end_generation()
{
    EvolutionaryOptimizer::end_generation();
}

void ProblemOptimizer::end_training()
{
    problem->disconnect("ended", Callable(this, "on_individual_ended"));
    auto genome = ea->get_best_individual();
    // UtilityFunctions::print(static_cast<String>("Final genome: " + Utils::vector_to_string_float(genome)));

    if (save_state)
    {
        auto state = EA::EvolutionaryAlgorithmState();
        state.genome = genome;
        state.save_json(stored_state);
        UtilityFunctions::print("Saved State.");
    }

    EvolutionaryOptimizer::end_training();
}

void godot::ProblemOptimizer::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_problem", "problem"), &ProblemOptimizer::set_problem);
    ClassDB::bind_method(D_METHOD("get_problem"), &ProblemOptimizer::get_problem);
    ClassDB::bind_method(D_METHOD("set_population_size", "population_size"), &ProblemOptimizer::set_population_size);
    ClassDB::bind_method(D_METHOD("get_population_size"), &ProblemOptimizer::get_population_size);
    ClassDB::bind_method(D_METHOD("on_individual_ended", "fitness"), &ProblemOptimizer::on_individual_ended);
    ClassDB::bind_method(D_METHOD("run_individual"), &ProblemOptimizer::run_individual);

    ClassDB::bind_method(D_METHOD("set_stored_state", "stored_state"), &ProblemOptimizer::set_stored_state);
    ClassDB::bind_method(D_METHOD("get_stored_state"), &ProblemOptimizer::get_stored_state);
    ClassDB::bind_method(D_METHOD("set_use_existing_state", "use_existing_state"), &ProblemOptimizer::set_use_existing_state);
    ClassDB::bind_method(D_METHOD("get_use_existing_state"), &ProblemOptimizer::get_use_existing_state);
    ClassDB::bind_method(D_METHOD("set_save_state", "save_state"), &ProblemOptimizer::set_save_state);
    ClassDB::bind_method(D_METHOD("get_save_state"), &ProblemOptimizer::get_save_state);

    ClassDB::bind_method(D_METHOD("set_multithreading", "multithreading"), &ProblemOptimizer::set_multithreading);
    ClassDB::bind_method(D_METHOD("get_multithreading"), &ProblemOptimizer::get_multithreading);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "problem", PROPERTY_HINT_NODE_TYPE, "OptimizableProblem"),
                 "set_problem", "get_problem");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "population_size"), "set_population_size", "get_population_size");

    ADD_GROUP("Stored State", "stored_");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "stored_state", PROPERTY_HINT_RESOURCE_TYPE, "JSON"),
                 "set_stored_state", "get_stored_state");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stored_use_existing"), "set_use_existing_state",
                 "get_use_existing_state");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "stored_save_state"), "set_save_state", "get_save_state");

    ADD_GROUP("Multithreading", "multithreading_");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "multithreading_enabled"), "set_multithreading", "get_multithreading");
}

godot::ProblemOptimizer::ProblemOptimizer() : EvolutionaryOptimizer()
{
}

OptimizableProblem *godot::ProblemOptimizer::get_problem() const
{
    return problem;
}

void godot::ProblemOptimizer::set_problem(OptimizableProblem *value)
{
    problem = value;
}

int godot::ProblemOptimizer::get_population_size() const
{
    return population_size;
}

void godot::ProblemOptimizer::set_population_size(int value)
{
    population_size = std::max(1, value);
}

Ref<JSON> godot::ProblemOptimizer::get_stored_state() const
{
    return stored_state;
}

void godot::ProblemOptimizer::set_stored_state(const Ref<JSON> &value)
{
    stored_state = value;
    notify_property_list_changed();
}

bool godot::ProblemOptimizer::get_use_existing_state() const
{
    return use_existing_state;
}

void godot::ProblemOptimizer::set_use_existing_state(const bool value)
{
    use_existing_state = value;
}

bool godot::ProblemOptimizer::get_save_state() const
{
    return save_state;
}

void godot::ProblemOptimizer::set_save_state(const bool value)
{
    save_state = value;
}

bool godot::ProblemOptimizer::get_multithreading() const
{
    return multithreading;
}

void godot::ProblemOptimizer::set_multithreading(bool value)
{
    multithreading = value;
}
