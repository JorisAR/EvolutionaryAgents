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
        ClassDB::bind_method(D_METHOD("get_lower_bound"), &EvolutionaryAlgorithmParameters::get_lower_bound);
        ClassDB::bind_method(D_METHOD("set_lower_bound", "lower_bound"), &EvolutionaryAlgorithmParameters::set_lower_bound);
        ClassDB::bind_method(D_METHOD("get_upper_bound"), &EvolutionaryAlgorithmParameters::get_upper_bound);
        ClassDB::bind_method(D_METHOD("set_upper_bound", "upper_bound"), &EvolutionaryAlgorithmParameters::set_upper_bound);

        ClassDB::bind_method(D_METHOD("get_use_bound"), &EvolutionaryAlgorithmParameters::get_use_bound);
        ClassDB::bind_method(D_METHOD("set_use_bound", "use_bound"), &EvolutionaryAlgorithmParameters::set_use_bound);

        ADD_GROUP("bounds", "bound");
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "bound_enabled"), "set_use_bound", "get_use_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bound_lower"), "set_lower_bound", "get_lower_bound");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bound_upper"), "set_upper_bound", "get_upper_bound");
    }

    void _init()
    {
        use_bound_ = false;
        lower_bound_ = 0.0f;
        upper_bound_ = 1.0f;
    }

    virtual ~EvolutionaryAlgorithmParameters()
    {
    }
    virtual EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) = 0;

    float get_lower_bound() const
    {
        return lower_bound_;
    }
    void set_lower_bound(float lower_bound)
    {
        lower_bound_ = lower_bound;
    }

    float get_upper_bound() const
    {
        return upper_bound_;
    }
    void set_upper_bound(float upper_bound)
    {
        upper_bound_ = upper_bound;
    }

    bool get_use_bound() const
    {
        return use_bound_;
    }
    void set_use_bound(bool use_bound)
    {
        use_bound_ = use_bound;
    }

  protected:
    float lower_bound_ = 0.0f;
    float upper_bound_ = 1.0f;
    bool use_bound_ = false;
};
} // namespace godot

#endif
