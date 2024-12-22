#include "evolutionary_gym.h"

using namespace godot;

void EvolutionaryGym::start_training()
{
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

    for (Agent *agent : agent_vector_)
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
        agent->start_game();
    }
}

void EvolutionaryGym::on_agent_ended()
{
    std::lock_guard<std::mutex> lock(agent_ended_mutex);

    active_agent_count--;
    if (active_agent_count == 0 && training)
    {
        end_generation();
    }
}

void EvolutionaryGym::end_generation()
{
    for (Agent *agent : agent_vector_)
    {
        register_fitness(agent->get_fitness());
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

    for (Agent *agent : agent_vector_)
    {
        agent->disconnect("ended", Callable(this, "on_agent_ended"));
    }

    EvolutionaryOptimizer::end_training();
}
