#include "sep_cma_es.h"

// implementation based on: https://github.com/CyberAgentAILab/cmaes/blob/main/cmaes/_sepcma.py
namespace EA
{

const float EPS = 1e-8f;
const float SIGMA_MAX = 1e32f;

// Function to compute the Euclidean norm (L2 norm) of a vector
double compute_norm(const std::vector<float> &vec)
{
    double inner_product = std::abs(std::inner_product(vec.begin(), vec.end(), vec.begin(), 0.0));
    if (inner_product < EPS)
    {
        return EPS;
    }
    return std::sqrt(inner_product);
}

SepCMAES::SepCMAES(int population_size, int individual_size, float sigma)
    : EvolutionaryAlgorithm(population_size, individual_size), sigma_(sigma)
{
    // init random
    gen = std::mt19937(rd());
    dist = std::normal_distribution<float>(0.0f, 1.0f);

    // init parameters
    mu_ = std::floor(population_size / 2.0f);

    auto initial_value = 0.0f;
    if(use_bound)
        initial_value = (lower_bound + upper_bound) / 2.0f;

    mean_ = std::vector<float>(individual_size, initial_value);
    D_ = std::vector<float>(individual_size, 1.0f);
    C_ = std::vector<float>(individual_size, 1.0f);
    p_sigma_ = std::vector<float>(individual_size, 0.0f);
    pc_ = std::vector<float>(individual_size, 0.0f);
    weights_ = std::vector<float>(mu_, 0.0f);
    float w_sum = 0.0f;
    for (int i = 0; i < mu_; i++)
    {
        weights_[i] = std::log(mu_ + 1.0f) - std::log(i + 1.0f);
        w_sum += weights_[i];
    }
    float w_squared_sum = 0.0f;
    weights_sum_ = 0.0f;
    for (int i = 0; i < mu_; i++)
    {
        weights_[i] = weights_[i] / w_sum;

        weights_sum_ += weights_[i];
        w_squared_sum += weights_[i] * weights_[i];
    }
    mu_eff_ = 1 / w_squared_sum;

    // learning rate for the rank-one update
    float alpha_cov = 2.0f;
    c1_ = alpha_cov / (std::pow(individual_size + 1.3f, 2.0f) + mu_eff_);

    // learning rate for the rank-mu update
    float cmu_full = 2.0f / mu_eff_ /
                     (std::pow(individual_size + std::sqrt(2.0f), 2.0f) +
                      (1.0f - 1.0f / mu_eff_) *
                          std::min(1.0f, (2.0f * mu_eff_ - 1.0f) / (std::pow(individual_size + 2.0f, 2.0f) + mu_eff_)));
    cmu_ = (individual_size + 2.0f) / 3.0f * cmu_full;
    cm_ = 1.0f;

    // learning rate for the cumulation for the step-size control
    c_sigma_ = (mu_eff_ + 2.0f) / (individual_size + mu_eff_ + 3.0f); // this should be < 1
    d_sigma_ = 1.0f + 2.0f * std::max(0.0f, std::sqrt((mu_eff_ - 1.0f) / (individual_size + 1.0f)) - 1.0f) + c_sigma_;

    // learning rate for the cumulation for the step-size control
    cc_ = 4.0f / (individual_size + 4.0f);

    // E||N(0, I)|| (p.28)
    chi_n_ = std::sqrt(individual_size) *
             (1.0f - (1.0f / (4.0f * individual_size)) + 1.0f / (21.0f * individual_size * individual_size));

    g_ = 0;
    population = generate_population();
}

void SepCMAES::set_starting_point(const std::vector<float> &individual)
{
    mean_ = individual;
    population = generate_population();
    population[0] = individual;
}

std::vector<std::vector<float>> SepCMAES::generate_population()
{
    population = std::vector<std::vector<float>>(population_size, std::vector<float>(individual_size));
    for (int i = 0; i < population_size; ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            auto v = mean_[j] + sigma_ * D_[j] * dist(gen); // ~ N(m, σ^2 C)
            if (use_bound)
                v = std::max(lower_bound, std::min(v, upper_bound));

            population[i][j] = v;
        }
    }
    return population;
}

void SepCMAES::evolve(const std::vector<float> &fitness)
{
    g_++;

    std::vector<std::pair<std::vector<float>, float>> solutions;
    for (int i = 0; i < population_size; ++i)
        solutions.push_back(std::make_pair(population[i], fitness[i]));
    // Sort solutions based on fitness values
    std::sort(solutions.begin(), solutions.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

    // rescale population as if they're distributed ~ N(0, C)
    for (int i = 0; i < solutions.size(); ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            solutions[i].first[j] = (solutions[i].first[j] - mean_[j]) / sigma_;
        }
    }

    // Selection and recombination
    std::vector<float> y_w(individual_size, 0.0f);
    for (int i = 0; i < mu_; ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            y_w[j] += solutions[i].first[j] * weights_[i];
        }
    }
    for (int j = 0; j < individual_size; ++j)
    {
        mean_[j] += cm_ * sigma_ * y_w[j];
    }

    // Update step-size control
    for (int j = 0; j < individual_size; ++j)
    {
        p_sigma_[j] =
            (1 - c_sigma_) * p_sigma_[j] + std::sqrt(c_sigma_ * (2.0f - c_sigma_) * mu_eff_) * (y_w[j] / D_[j]);
    }

    float norm_p_sigma = compute_norm(p_sigma_);
    // std::cout << "norm_p_sigma: " << norm_p_sigma << std::endl;
    sigma_ *= std::exp((c_sigma_ / d_sigma_) * (norm_p_sigma / chi_n_ - 1.0f));
    sigma_ = std::min(SIGMA_MAX, sigma_);
    // std::cout << "sigma_after_update: " << sigma_ << std::endl;

    // covariance matrix adaptation
    float h_sigma_cond_left = norm_p_sigma / std::sqrt(1.0f - std::pow(1.0f - c_sigma_, 2.0f * (g_ + 1.0f)));
    float h_sigma_cond_right = (1.4f + 2.0f / (individual_size + 1.0f)) * chi_n_;
    float h_sigma = h_sigma_cond_left < h_sigma_cond_right ? 1.0f : 0.0f;
    // std::cout << "h_sigma: " << h_sigma << std::endl;

    float pc_update = h_sigma * std::sqrt(cc_ * (2.0f - cc_) * mu_eff_);
    // std::cout << "pc_update: " << pc_update << std::endl;
    std::vector<float> rank_one(individual_size, 0.0f);
    for (int j = 0; j < individual_size; ++j)
    {
        pc_[j] = (1.0f - cc_) * pc_[j] + pc_update * y_w[j];
        rank_one[j] = pc_[j] * pc_[j];
    }

    float delta_h_sigma = (1.0f - h_sigma) * cc_ * (2.0f - cc_); // TODO ensure this is <= 1!!!
    // std::cout << "delta_h_sigma: " << delta_h_sigma << std::endl;
    std::vector<float> rank_mu(individual_size, 0.0f);
    for (int i = 0; i < mu_; ++i)
    {
        for (int j = 0; j < individual_size; ++j)
        {
            rank_mu[j] += weights_[i] * solutions[i].first[j] * solutions[i].first[j];
        }
    }
    for (int j = 0; j < individual_size; ++j)
    {
        // std::cout << "rank_mu: " << rank_mu[j] << ", rank_one: " << rank_one[j] << std::endl;
        C_[j] =
            (1.0f + c1_ * delta_h_sigma - c1_ - cmu_ * weights_sum_) * C_[j] + c1_ * rank_one[j] + cmu_ * rank_mu[j];

        // update D:
        if (C_[j] <= EPS)
            D_[j] = std::sqrt(EPS);
        else
            D_[j] = std::sqrt(C_[j]);
    }

    // Update the population
    population = generate_population();
}

std::vector<float> SepCMAES::get_best_individual()
{
    return mean_;
}

} // namespace EA
