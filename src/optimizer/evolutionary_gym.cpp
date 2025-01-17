#include "evolutionary_gym.h"

using namespace godot;

void EvolutionaryGym::start_training()
{
    if(!auto_start) return;

    if (neural_network == nullptr)
    {
        UtilityFunctions::printerr("No neural network selected, aborted training.");
        return;
    }
    if (ea_params == nullptr)
    {
        UtilityFunctions::printerr("No evolutionary algorithm selected, aborted training.");
        return;
    }

    active_agent_count = 0;
    population_size = 0;

    for (EvolutionaryAgent *agent : agent_vector_)
    {
        population_size += 1;
        agent->connect("ended", Callable(this, "on_agent_ended"));
    }

    layers = neural_network->get_layers();
    auto parameters = NeuralNetwork::calculate_total_parameters(layers);
    genome_size = parameters[0] + parameters[1];

    // Initialize the evolutionary strategy
    if (ea == nullptr && neural_network->get_use_existing_network())
    {
        auto params = EA::EvolutionaryAlgorithmState::from_json(neural_network->get_stored_network());
        if (params != nullptr)
        {
            auto genome = params->genome;
            if (genome.size() == genome_size)
            {
                ea = ea_params->get_evolutionary_algorithm(population_size, genome_size);
                ea->set_starting_point(genome);
                UtilityFunctions::print("Initializing population with pre-trained network");
            }
        }
    }

    EvolutionaryOptimizer::start_training();
}

void EvolutionaryGym::start_generation()
{
    EvolutionaryOptimizer::start_generation();
    active_agent_count = population_size;
    for (size_t i = 0; i < population_size; i++)
    {
        auto agent = agent_vector_[i];
        agent->update(layers, population[i]);
        agent->start();
    }
}

void EvolutionaryGym::on_agent_ended()
{
    std::lock_guard<std::mutex> lock(agent_ended_mutex);

    active_agent_count--;
    if (active_agent_count == 0 && training && !run_once)
    {
        end_generation();
    }
}

void EvolutionaryGym::end_generation()
{
    int i = 0;
    for (EvolutionaryAgent *agent : agent_vector_)
    {
        register_fitness(i++, agent->get_fitness());
    }
    EvolutionaryOptimizer::end_generation();
}

void EvolutionaryGym::end_training()
{
    //neural_network->save_neural_network_to_file(ea->get_best_individual());
    auto genome = ea->get_best_individual();
    
    if (true && neural_network.is_valid())//(save_state)
    {
        auto state = EA::EvolutionaryAlgorithmState();
        state.genome = genome;
        state.layers = layers;
        state.save_json(neural_network->get_stored_network());
        UtilityFunctions::print("Saved State.");
    }

    for (EvolutionaryAgent *agent : agent_vector_)
    {
        agent->disconnect("ended", Callable(this, "on_agent_ended"));
    }

    EvolutionaryOptimizer::end_training();
}

void godot::EvolutionaryGym::_bind_methods()
{
    // ClassDB::bind_method(D_METHOD("on_agent_ended"), &EvolutionaryGym::on_agent_ended);
    ClassDB::bind_method(D_METHOD("set_agents", "agents"), &EvolutionaryGym::set_agents);
    ClassDB::bind_method(D_METHOD("get_agents"), &EvolutionaryGym::get_agents);

    ClassDB::bind_method(D_METHOD("set_neural_network", "neural network"), &EvolutionaryGym::set_neural_network);
    ClassDB::bind_method(D_METHOD("get_neural_network"), &EvolutionaryGym::get_neural_network);

    ClassDB::bind_method(D_METHOD("get_physics_ticks_per_second"), &EvolutionaryGym::get_physics_ticks_per_second);
    ClassDB::bind_method(D_METHOD("set_physics_ticks_per_second", "value"), &EvolutionaryGym::set_physics_ticks_per_second);

    ClassDB::bind_method(D_METHOD("get_max_fps"), &EvolutionaryGym::get_max_fps);
    ClassDB::bind_method(D_METHOD("set_max_fps", "value"), &EvolutionaryGym::set_max_fps);

    ClassDB::bind_method(D_METHOD("get_time_scale"), &EvolutionaryGym::get_time_scale);
    ClassDB::bind_method(D_METHOD("set_time_scale", "value"), &EvolutionaryGym::set_time_scale);

    ClassDB::bind_method(D_METHOD("get_auto_start"), &EvolutionaryGym::get_auto_start);
    ClassDB::bind_method(D_METHOD("set_auto_start", "value"), &EvolutionaryGym::set_auto_start);

    ClassDB::bind_method(D_METHOD("get_run_once"), &EvolutionaryGym::get_run_once);
    ClassDB::bind_method(D_METHOD("set_run_once", "value"), &EvolutionaryGym::set_run_once);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_start"), "set_auto_start", "get_auto_start");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "run_once"), "set_run_once", "get_run_once");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "agents", PROPERTY_HINT_TYPE_STRING,
                              String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":EvolutionaryAgent"),
                 "set_agents", "get_agents");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "neural_network_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                              "NeuralNetworkParameters"),
                 "set_neural_network", "get_neural_network");

    ADD_GROUP("Time", "");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_ticks_per_second"), "set_physics_ticks_per_second",
                 "get_physics_ticks_per_second");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_fps"), "set_max_fps", "get_max_fps");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "time_scale"), "set_time_scale", "get_time_scale");
}

godot::EvolutionaryGym::EvolutionaryGym() : EvolutionaryOptimizer()
{
}

void godot::EvolutionaryGym::set_agents(const TypedArray<EvolutionaryAgent> value)
{
    agents = value;
    agent_vector_.clear();
    for (size_t i = 0; i < agents.size(); i++)
    {
        EvolutionaryAgent *agent = Object::cast_to<EvolutionaryAgent>(agents[i]);
        if (agent)
        {
            agent_vector_.push_back(agent);
        }
    }
}

TypedArray<EvolutionaryAgent> godot::EvolutionaryGym::get_agents() const
{
    return agents;
}

bool godot::EvolutionaryGym::get_auto_start() const
{
    return auto_start;
}

void godot::EvolutionaryGym::set_auto_start(const bool value)
{
    auto_start = value;
}

bool godot::EvolutionaryGym::get_run_once() const
{
    return run_once;
}

void godot::EvolutionaryGym::set_run_once(const bool value)
{
    run_once = value;
}


void godot::EvolutionaryGym::set_neural_network(Ref<NeuralNetworkParameters> value)
{
    neural_network = value;
}

Ref<NeuralNetworkParameters> godot::EvolutionaryGym::get_neural_network() const
{
    return neural_network;
}

int godot::EvolutionaryGym::get_current_generation() const
{
    return current_generation;
}

int godot::EvolutionaryGym::get_physics_ticks_per_second() const
{
    return physics_ticks_per_second;
}

void godot::EvolutionaryGym::set_physics_ticks_per_second(int value)
{
    physics_ticks_per_second = value;
    update_engine_settings();
}

int godot::EvolutionaryGym::get_max_fps() const
{
    return max_fps;
}

void godot::EvolutionaryGym::set_max_fps(int value)
{
    max_fps = value;
    update_engine_settings();
}

int godot::EvolutionaryGym::get_time_scale() const
{
    return time_scale;
}

void godot::EvolutionaryGym::set_time_scale(int value)
{
    time_scale = value;
    update_engine_settings();
}

void godot::EvolutionaryGym::update_engine_settings()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        return;
    }

    Engine::get_singleton()->set_max_fps(max_fps);
    Engine::get_singleton()->set_time_scale(time_scale);
    Engine::get_singleton()->set_physics_ticks_per_second(physics_ticks_per_second * time_scale);
    Engine::get_singleton()->set_max_physics_steps_per_frame(
        std::max(1, static_cast<int>(std::ceil(static_cast<float>(physics_ticks_per_second * time_scale) /
                                               static_cast<float>(max_fps)))));
}
