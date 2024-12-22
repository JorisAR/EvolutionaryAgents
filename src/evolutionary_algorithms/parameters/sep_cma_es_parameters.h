#ifndef SEP_CMA_ES_PARAMETERS_H
#define SEP_CMA_ES_PARAMETERS_H

#include "evolutionary_algorithm_parameters.h"
#include "sep_cma_es.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot
{

class SepCMAESParameters : public EvolutionaryAlgorithmParameters
{
    GDCLASS(SepCMAESParameters, EvolutionaryAlgorithmParameters)

  public:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_sigma"), &SepCMAESParameters::get_sigma);
        ClassDB::bind_method(D_METHOD("set_sigma", "sigma"), &SepCMAESParameters::set_sigma);

        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sigma"), "set_sigma", "get_sigma");
    }

    SepCMAESParameters()
    {
    }
    ~SepCMAESParameters()
    {
    }

    void _init()
    {
        sigma_ = 0.5;
    }

    float get_sigma() const
    {
        return sigma_;
    }
    void set_sigma(float sigma)
    {
        sigma_ = sigma;
    }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override
    {
        auto ea = new EA::SepCMAES(population_size, individual_size, sigma_);
        if (get_use_bound())
            ea->set_bounds(get_lower_bound(), get_upper_bound());
        return ea;
    }

  private:
    float sigma_ = 0.5;
};

} // namespace godot

#endif
