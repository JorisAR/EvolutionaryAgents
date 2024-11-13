#ifndef GDEvolutionaryAgents_REGISTER_TYPES_H
#define GDEvolutionaryAgents_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include "evolutionary_strategy_parameters.h"
#include "firefly_algorithm_parameters.h"
#include "gym/evolutionary_gym.h"
#include "neural_network_parameters.h"
#include "sep_cma_es_parameters.h"
#include "sensor3d.h"
#include "depth_sensor3d.h"

using namespace godot;

void initialize_evolutionary_agents_module(ModuleInitializationLevel p_level);
void uninitialize_evolutionary_agents_module(ModuleInitializationLevel p_level);

#endif