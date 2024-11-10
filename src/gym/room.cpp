#include "Room.h"
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Room::start_game()
{
    // Initialize agents and game objects
    emit_signal("started");
}

std::vector<Agent *> godot::Room::get_agents_vector() const
{
    auto result = std::vector<Agent *>();
    for (int i = 0; i < agents.size(); ++i)
    {
        Agent *agent = Object::cast_to<Agent>(agents[i]);
        if (agent)
        {
            result.push_back(agent);
        }
    }
    return result;
}
