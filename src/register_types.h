#ifndef GDEvolutionaryAgents_REGISTER_TYPES_H
#define GDEvolutionaryAgents_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_evolutionary_agents_module(ModuleInitializationLevel p_level);
void uninitialize_evolutionary_agents_module(ModuleInitializationLevel p_level);

#endif