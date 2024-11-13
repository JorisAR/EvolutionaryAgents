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
    }

    virtual ~EvolutionaryAlgorithmParameters()
    {
    }
    virtual EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) = 0;
};
} // namespace godot

#endif
