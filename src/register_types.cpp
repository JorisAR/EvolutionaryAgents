#include "register_types.h"

#include "optimizer/evolutionary_optimizer.h"
#include "evolutionary_strategy_parameters.h"
#include "genetic_algorithm_parameters.h"
#include "stochastic_ascent_ga_parameters.h"
#include "optimizer/gym.h"
#include "neural_network_parameters.h"
#include "sep_cma_es_parameters.h"
#include "optimizer/sensors/sensor3d.h"
#include "optimizer/sensors/depth_sensor3d.h"
#include "optimizer/problem_optimizer.h"

using namespace godot;

void initialize_evolutionary_agents_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        // Neural Networks
        GDREGISTER_CLASS(NeuralNetworkParameters);

        // EAs
        GDREGISTER_ABSTRACT_CLASS(EvolutionaryOptimizer);
        GDREGISTER_CLASS(ProblemOptimizer);
        GDREGISTER_CLASS(OptimizableProblem);

        GDREGISTER_ABSTRACT_CLASS(EvolutionaryAlgorithmParameters);
        GDREGISTER_CLASS(EvolutionaryStrategyParameters);
        GDREGISTER_CLASS(SepCMAESParameters);
        GDREGISTER_CLASS(GeneticAlgorithmParameters);
        GDREGISTER_CLASS(StochasticAscentGAParameters);

        // Gym
        GDREGISTER_CLASS(Agent);
        GDREGISTER_CLASS(Gym);

        // Sensors
        GDREGISTER_ABSTRACT_CLASS(Sensor3D);
        GDREGISTER_CLASS(DepthSensor3D);
    }
}

void uninitialize_evolutionary_agents_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT evolutionary_agents_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                                const GDExtensionClassLibraryPtr p_library,
                                                                GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_evolutionary_agents_module);
        init_obj.register_terminator(uninitialize_evolutionary_agents_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}