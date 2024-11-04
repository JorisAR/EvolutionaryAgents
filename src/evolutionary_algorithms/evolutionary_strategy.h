#ifndef EVOLUTIONARY_STRATEGY_H
#define EVOLUTIONARY_STRATEGY_H

#include "evolutionary_algorithm.h"
#include <vector>

namespace EA {
    class EvolutionaryStrategy : public EvolutionaryAlgorithm {
    public:
        EvolutionaryStrategy(int population_size, int individual_size, float sigma, float alpha);
        
        void evolve(const std::vector<float>& fitness) override;

    private:
        float sigma_;
        float alpha_;
        std::vector<float> weights_;

        std::vector<std::vector<float>> generate_population();
    };
}

#endif
