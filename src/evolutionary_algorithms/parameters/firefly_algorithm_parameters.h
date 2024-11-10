#ifndef FIREFLY_ALGORITHM_PARAMETERS_H
#define FIREFLY_ALGORITHM_PARAMETERS_H

#include "evolutionary_algorithm_parameters.h"
#include "firefly_algorithm.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class FireflyAlgorithmParameters : public EvolutionaryAlgorithmParameters {
    GDCLASS(FireflyAlgorithmParameters, EvolutionaryAlgorithmParameters)

public:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_beta"), &FireflyAlgorithmParameters::get_beta);
        ClassDB::bind_method(D_METHOD("set_beta", "beta"), &FireflyAlgorithmParameters::set_beta);
        ClassDB::bind_method(D_METHOD("get_gamma"), &FireflyAlgorithmParameters::get_gamma);
        ClassDB::bind_method(D_METHOD("set_gamma", "gamma"), &FireflyAlgorithmParameters::set_gamma);
        ClassDB::bind_method(D_METHOD("get_alpha"), &FireflyAlgorithmParameters::get_alpha);
        ClassDB::bind_method(D_METHOD("set_alpha", "alpha"), &FireflyAlgorithmParameters::set_alpha);

        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "beta"), "set_beta", "get_beta");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gamma"), "set_gamma", "get_gamma");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "alpha"), "set_alpha", "get_alpha");
    }

    FireflyAlgorithmParameters() {}
    ~FireflyAlgorithmParameters() {}

    void _init() {
        beta_ = 1.0;
        gamma_ = 1.0;
        alpha_ = 0.2;
    }

    float get_beta() const {
        return beta_;
    }
    void set_beta(float beta) {
        beta_ = beta;
    }

    float get_gamma() const {
        return gamma_;
    }
    void set_gamma(float gamma) {
        gamma_ = gamma;
    }

    float get_alpha() const {
        return alpha_;
    }
    void set_alpha(float alpha) {
        alpha_ = alpha;
    }

    EA::EvolutionaryAlgorithm *get_evolutionary_algorithm(int population_size, int individual_size) override {
        return new EA::FireflyAlgorithm(population_size, individual_size, beta_, gamma_, alpha_);
    }

private:
    float beta_ = 1.0;
    float gamma_ = 1.0;
    float alpha_ = 0.2;
};

} // namespace godot

#endif
