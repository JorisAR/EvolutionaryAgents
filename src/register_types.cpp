#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include "evolutionary_strategy_parameters.h"
#include "firefly_algorithm_parameters.h"
#include "gym/evolutionary_gym.h"
#include "serializable_neural_network.h"
#include "sep_cma_es_parameters.h"

using namespace godot;

void initialize_evolutionary_agents_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    //Neural Networks
    GDREGISTER_CLASS(SerializableNeuralNetwork);

    // EAs
    GDREGISTER_ABSTRACT_CLASS(EvolutionaryAlgorithmParameters);
    GDREGISTER_CLASS(EvolutionaryStrategyParameters);
    GDREGISTER_CLASS(FireflyAlgorithmParameters);
    GDREGISTER_CLASS(SepCMAESParameters);
    

    // GDREGISTER_CLASS(GDExample);
    GDREGISTER_CLASS(Agent);
    GDREGISTER_CLASS(Room);
    GDREGISTER_CLASS(EvolutionaryGym);
}

void uninitialize_evolutionary_agents_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT example_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
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