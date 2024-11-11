#ifndef EVOLUTIONARY_ALGORITHM_PARAMETERS_H
#define EVOLUTIONARY_ALGORITHM_PARAMETERS_H

#include "evolutionary_algorithm.h"
#include <godot_cpp/classes/resource.hpp>

namespace godot
{
class EvolutionaryAlgorithmParameters : public Resource
{
    GDCLASS(EvolutionaryAlgorithmParameters, Resource)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_use_existing_network"), &EvolutionaryAlgorithmParameters::get_use_existing_network);
        ClassDB::bind_method(D_METHOD("set_use_existing_network", "use_existing_network_"), &EvolutionaryAlgorithmParameters::set_use_existing_network);

        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_existing_network"), "set_use_existing_network", "get_use_existing_network");
    }

    float get_use_existing_network() const
    {
        return use_existing_network_;
    }
    void set_use_existing_network(float value)
    {
        use_existing_network_ = value;
    }

    virtual ~EvolutionaryAlgorithmParameters()
    {
    }
    virtual EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) = 0;

  private:
    bool use_existing_network_ = false;
};
} // namespace godot

#endif
