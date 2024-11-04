#ifndef EVOLUTIONARY_ALGORITHM_H
#define EVOLUTIONARY_ALGORITHM_H

#include <vector>

namespace EA {
    class EvolutionaryAlgorithm {
    public:
        EvolutionaryAlgorithm(int population_size, int individual_size);
        virtual ~EvolutionaryAlgorithm() {}

        std::vector<std::vector<float>> get_population() const;
        
        virtual void evolve(const std::vector<float>& fitness) = 0;

    protected:
        int population_size;
        int individual_size;
        std::vector<std::vector<float>> population;

        virtual void initialize_population();
    };
}

#endif
