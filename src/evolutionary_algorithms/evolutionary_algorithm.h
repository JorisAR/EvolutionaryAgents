#ifndef EVOLUTIONARY_ALGORITHM_H
#define EVOLUTIONARY_ALGORITHM_H

#include <godot_cpp/classes/resource.hpp>
#include <vector>

namespace EA {
    class EvolutionaryAlgorithm {

    public:
        EvolutionaryAlgorithm(int population_size, int individual_size);
        virtual ~EvolutionaryAlgorithm() {}

        std::vector<std::vector<float>> get_population() const;
        virtual void evolve(const std::vector<float>& fitness) = 0;
        virtual std::vector<float> get_best_individual() = 0;
        void set_population(const std::vector<float> individual);

    protected:
        int population_size;
        int individual_size;
        std::vector<std::vector<float>> population;

        virtual void initialize_population();
    };
}

#endif
