#ifndef FIREFLY_ALGORITHM_H
#define FIREFLY_ALGORITHM_H

#include "evolutionary_algorithm.h"
#include <vector>

namespace EA {
class FireflyAlgorithm : public EvolutionaryAlgorithm {
public:
    FireflyAlgorithm(int population_size, int individual_size, float beta, float gamma, float alpha);
    ~FireflyAlgorithm() {}

    void evolve(const std::vector<float>& fitness) override;
    std::vector<float> get_best_individual() override;

private:
    float beta_;
    float gamma_;
    float alpha_;
    std::vector<std::vector<float>> generate_population();
    void update_firefly(std::vector<float>& firefly_i, const std::vector<float>& firefly_j, float attractiveness);
};
} // namespace EA

#endif
