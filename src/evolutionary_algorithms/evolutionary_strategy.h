#ifndef EVOLUTIONARY_STRATEGY_H
#define EVOLUTIONARY_STRATEGY_H

#include "evolutionary_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <vector>

namespace EA
{
class EvolutionaryStrategy : public EvolutionaryAlgorithm
{
  public:
    EvolutionaryStrategy(int population_size, int individual_size, float sigma, float alpha);
    ~EvolutionaryStrategy()
    {
    }
    void evolve(const std::vector<float> &fitness) override;
    std::vector<float> get_best_individual() override;
    void set_starting_point(const std::vector<float>& individual) override;

  private:
    float sigma_;
    float alpha_;
    std::vector<float> weights_;
    std::vector<std::vector<float>> generate_population();
};
} // namespace EA

#endif
