#ifndef SEP_CMA_ES_H
#define SEP_CMA_ES_H

#include "evolutionary_algorithm.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace EA {
class SepCMAES : public EvolutionaryAlgorithm {
public:
    SepCMAES(int population_size, int individual_size, float sigma);
    ~SepCMAES() {}

    void evolve(const std::vector<float>& fitness) override;
    std::vector<float> get_best_individual() override;

private:
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<float> dist;

    std::vector<float> weights_;

    std::vector<float> D_; //eigendecomposition of C
    std::vector<float> C_; //diagonal covariance matrix

    std::vector<float> mean_; 
    float sigma_;
    std::vector<float> p_sigma_; 
    std::vector<float> pc_; 
    float c_sigma_;
    float d_sigma_;
    int mu_;
    float mu_eff_;
    float cc_;
    float c1_;
    float cmu_;
    float cm_;
    float chi_n_;
    float g_; //generation
    float weights_sum_;

    std::vector<std::vector<float>> generate_population();
};
} // namespace EA

#endif
