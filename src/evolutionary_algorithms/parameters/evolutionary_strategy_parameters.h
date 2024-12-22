#ifndef EVOLUTIONARY_STRATEGY_PARAMETERS_H
#define EVOLUTIONARY_STRATEGY_PARAMETERS_H

#include "evolutionary_algorithm_parameters.h"
#include "evolutionary_strategy.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
class EvolutionaryStrategyParameters : public EvolutionaryAlgorithmParameters
{
    GDCLASS(EvolutionaryStrategyParameters, EvolutionaryAlgorithmParameters)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_sigma"), &EvolutionaryStrategyParameters::get_sigma);
        ClassDB::bind_method(D_METHOD("set_sigma", "sigma"), &EvolutionaryStrategyParameters::set_sigma);
        ClassDB::bind_method(D_METHOD("get_alpha"), &EvolutionaryStrategyParameters::get_alpha);
        ClassDB::bind_method(D_METHOD("set_alpha", "alpha"), &EvolutionaryStrategyParameters::set_alpha);

        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sigma"), "set_sigma", "get_sigma");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha"), "set_alpha", "get_alpha");
    }

    EvolutionaryStrategyParameters()
    {
    }
    ~EvolutionaryStrategyParameters()
    {
    }

    void _init()
    {
        sigma_ = 0.1;
        alpha_ = 0.01;
    }

    float get_sigma() const
    {
        return sigma_;
    }
    void set_sigma(float sigma)
    {
        sigma_ = sigma;
    }

    float get_alpha() const
    {
        return alpha_;
    }
    void set_alpha(float alpha)
    {
        alpha_ = alpha;
    }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override
    {
        auto ea = new EA::EvolutionaryStrategy(population_size, individual_size, sigma_, alpha_);
        if (get_use_bound())
            ea->set_bounds(get_lower_bound(), get_upper_bound());
        return ea;
    }

  private:
    float sigma_ = 0.1;
    float alpha_ = 0.01;
};
} // namespace godot

#endif
