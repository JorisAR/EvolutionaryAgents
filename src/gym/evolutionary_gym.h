#ifndef EvolutionaryGym_H
#define EvolutionaryGym_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <vector>

#include "Room.h"
#include "evolutionary_algorithm_parameters.h"
#include "evolutionary_strategy.h"
#include "logger.h"
#include "serializable_neural_network.h"
#include "utils.h"

namespace godot
{

class EvolutionaryGym : public Node
{
    GDCLASS(EvolutionaryGym, Node)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("start_training"), &EvolutionaryGym::start_training);
        ClassDB::bind_method(D_METHOD("end_training"), &EvolutionaryGym::end_training);
        ClassDB::bind_method(D_METHOD("start_generation"), &EvolutionaryGym::start_generation);
        ClassDB::bind_method(D_METHOD("end_generation"), &EvolutionaryGym::end_generation);

        ClassDB::bind_method(D_METHOD("on_room_ended"), &EvolutionaryGym::on_room_ended);
        ClassDB::bind_method(D_METHOD("set_rooms", "rooms"), &EvolutionaryGym::set_rooms);
        ClassDB::bind_method(D_METHOD("get_rooms"), &EvolutionaryGym::get_rooms);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "rooms", PROPERTY_HINT_TYPE_STRING,
                                  String::num(Variant::OBJECT) + "/" + String::num(PROPERTY_HINT_NODE_TYPE) + ":Room"),
                     "set_rooms", "get_rooms"); // The list of rooms the gym will use during training.

        ClassDB::bind_method(D_METHOD("set_verbose", "verbose"), &EvolutionaryGym::set_verbose);
        ClassDB::bind_method(D_METHOD("get_verbose"), &EvolutionaryGym::get_verbose);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "verbose"), "set_verbose", "get_verbose");

        ClassDB::bind_method(D_METHOD("set_log", "log"), &EvolutionaryGym::set_log);
        ClassDB::bind_method(D_METHOD("get_log"), &EvolutionaryGym::get_log);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "log"), "set_log", "get_log");

        ClassDB::bind_method(D_METHOD("get_current_generation"), &EvolutionaryGym::get_current_generation);
        ADD_SIGNAL(MethodInfo("generation_ended")); // Signal emitted every generation.

        ClassDB::bind_method(D_METHOD("set_ea_params", "ea parameters"), &EvolutionaryGym::set_ea_params);
        ClassDB::bind_method(D_METHOD("get_ea_params"), &EvolutionaryGym::get_ea_params);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "evolutionary_algorithm_parameters", PROPERTY_HINT_RESOURCE_TYPE,
                                  "EvolutionaryAlgorithmParameters"),
                     "set_ea_params", "get_ea_params"); // The evolutionary algorithm the gym will use as an optimizer.

        ClassDB::bind_method(D_METHOD("set_neural_network", "neural network"), &EvolutionaryGym::set_neural_network);
        ClassDB::bind_method(D_METHOD("get_neural_network"), &EvolutionaryGym::get_neural_network);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "serializable_neural_network", PROPERTY_HINT_RESOURCE_TYPE,
                                  "SerializableNeuralNetwork"),
                     "set_neural_network", "get_neural_network"); // The neural network the gym will train. The gym will
                                                                  // overwrite the trained parameters when saved.
    }

    EvolutionaryGym() : ea(nullptr)
    {
    }
    ~EvolutionaryGym()
    {
        delete ea;
    }

    void _init()
    {
        current_generation = 0;
        active_room_count = 0;
        rooms.clear();
    }

    void start_training();
    void end_training();
    void start_generation();
    void on_room_ended();
    void end_generation();

    void _ready()
    {
        start_training();
    }

    void set_rooms(const TypedArray<Room> value)
    {
        rooms = value;
        _room_vector.clear();
        for (size_t i = 0; i < rooms.size(); i++)
        {
            Room *room = Object::cast_to<Room>(rooms[i]);
            if (room)
            {
                _room_vector.push_back(room);
            }
        }
    }
    TypedArray<Room> get_rooms() const
    {
        return rooms;
    }

    bool get_verbose() const
    {
        return verbose;
    }
    void set_verbose(const bool value)
    {
        verbose = value;
    }

    bool get_log() const
    {
        return log;
    }
    void set_log(const bool value)
    {
        log = value;
    }

    Ref<EvolutionaryAlgorithmParameters> get_ea_params() const
    {
        return ea_params;
    }

    void set_ea_params(Ref<EvolutionaryAlgorithmParameters> value)
    {
        ea_params = value;
    }

    void set_neural_network(Ref<SerializableNeuralNetwork> value)
    {
        neural_network = value;
    }

    Ref<SerializableNeuralNetwork> get_neural_network() const
    {
        return neural_network;
    }

    int get_current_generation() const
    {
        return current_generation;
    }

  private:
    TypedArray<Room> rooms;
    std::vector<Room *> _room_vector;
    int current_generation;
    int active_room_count;
    int agent_count;

    std::vector<int> layers;
    int weight_count;
    int bias_count;
    Ref<EvolutionaryAlgorithmParameters> ea_params;
    EA::EvolutionaryAlgorithm *ea;
    bool verbose = false;
    bool log = false;

    Logger* logger;
    Ref<SerializableNeuralNetwork> neural_network;
};

} // namespace godot

#endif
