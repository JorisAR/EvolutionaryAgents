#ifndef ROOM_H
#define ROOM_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>
#include "Agent.h"

namespace godot {

class Room : public Node {
    GDCLASS(Room, Node)

public:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("start_game"), &Room::start_game);
        ClassDB::bind_method(D_METHOD("set_agents", "agents"), &Room::set_agents);
        ClassDB::bind_method(D_METHOD("get_agents"), &Room::get_agents);
        ADD_SIGNAL(MethodInfo("started"));
        ADD_SIGNAL(MethodInfo("ended"));        
        ADD_PROPERTY(
                PropertyInfo(Variant::ARRAY, "agents",
                             PROPERTY_HINT_TYPE_STRING,
                             String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":Agent"),
                "set_agents", "get_agents");
    }

    Room() { }
    ~Room() { }

    void _init() {
        // Initialize room
    }    

    void start_game();
    void set_agents(const TypedArray<Agent> agents_array) { agents = agents_array; }
    TypedArray<Agent> get_agents() const { return agents; }
    std::vector<Agent*> get_agents_vector() const;

private:
    TypedArray<Agent> agents;
};

}

#endif
