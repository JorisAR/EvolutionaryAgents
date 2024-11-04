#include "test_problem.h"
#include <cmath>

float TestProblem::evaluate(const std::vector<float>& individual) {
    float sum = 0;
    for (auto gene : individual) {
        auto dif = 0.5 - gene;
        sum -= dif * dif;
    }
    return sum;
}
