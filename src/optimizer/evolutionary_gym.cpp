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
    current_generation = 0;
    agent_count = 0;

    for (Agent *agent : agent_vector_)
    {
        agent_count += 1;
        agent->connect("ended", Callable(this, "on_agent_ended"));
    }
    layers = neural_network->get_layers();
    auto parameters = NeuralNetwork::calculate_total_parameters(layers);
    weight_count = parameters[0];
    bias_count = parameters[1];

    // Initialize the evolutionary strategy
    if (ea == nullptr)
    {
        ea = ea_params->get_evolutionary_algorithm(agent_count, weight_count + bias_count);
        if (neural_network->get_use_existing_network())
        {
            auto params = neural_network->load_neural_network_from_file()->get_parameters();
            if (params.size() == weight_count + bias_count)
            {
                UtilityFunctions::print("Initializing population with pretrained-parameters");
                ea->set_starting_point(Utils::array_to_vector_float(params));
            }
        }
    }

    // Initialize the logger
    if (log)
    {
        auto log_path = neural_network->get_log_path();
        if (!log_path.is_empty())
        {
            logger = new Logger(log_path.utf8().get_data());
        }
    }

    start_generation();
}

void EvolutionaryGym::start_generation()
{
    current_generation++;
    // Generate population
    auto population = ea->get_population();
    int individual = 0;

    for (Agent *agent : agent_vector_)
    {
        agent->update(layers, population[individual]);
        individual++;
        agent->start_game();
        active_agent_count++;
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
    std::vector<float> fitness = std::vector<float>();
    float generation_best_fitness = 1e-32f;
    float generation_mean_fitness = 0;
    for (Agent *agent : agent_vector_)
    {
        auto f = agent->get_fitness();
        fitness.push_back(f);
        best_fitness = Math::max(best_fitness, f);
        generation_best_fitness = Math::max(generation_best_fitness, f);
        sum_fitness += f;
        generation_mean_fitness += f;
    }

    generation_mean_fitness /= agent_count;

    if (verbose && (current_generation) % debug_generation_interval == 0)
    {
        sum_fitness /= agent_count * debug_generation_interval;
        UtilityFunctions::print(static_cast<String>("Generation: " + String::num(current_generation) +
                                                    ", Best Fitness: " + String::num(best_fitness)) +
                                ", Mean Fitness: " + String::num(sum_fitness));

        float best_fitness = 1e-32f;
        float sum_fitness = 0.0f;
    }

    if (log && logger != nullptr)
    {
        logger->log_generation(generation_mean_fitness, generation_best_fitness);
    }

    ea->evolve(fitness);
    emit_signal("generation_ended");
    start_generation();
}

void EvolutionaryGym::end_training()
{
    UtilityFunctions::print("Training finished!");
    neural_network->save_neural_network_to_file(ea->get_best_individual());

    if (log && logger != nullptr)
    {
        logger->flush();
    }

    delete logger;
    logger = nullptr;

    delete ea;
    ea = nullptr;

    for (Agent *agent : agent_vector_)
    {
        agent->disconnect("ended", Callable(this, "on_agent_ended"));
    }
}
