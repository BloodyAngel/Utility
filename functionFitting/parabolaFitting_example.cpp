#include "parabolaFitting.hpp"
#include <assert.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

namespace parameter {
    static constexpr size_t NumPoints = 100;
    static constexpr double XMin = -10.0;
    static constexpr double XMax = +10.0;

    static constexpr double value_adjustment = 0.1;
} // namespace parameter

static std::mt19937 mersenne((std::random_device())());

static auto getRandomXValues() {
    std::vector<double> x_values(parameter::NumPoints);
    std::normal_distribution dis(parameter::XMin, parameter::XMax);
    for (auto& e : x_values)
        e = dis(mersenne);
    return x_values;
}

static auto getCalculatedYValues(double a, double b, double c,
                                 const std::vector<double>& values) {
    std::vector<double> y_values(values.size());
    std::transform(std::begin(values), std::end(values), std::begin(y_values),
                   [&a, &b, &c](double x) { return a * x * x + b * x + c; });
    return y_values;
}

static void fitParabola(const std::vector<double>& x,
                        const std::vector<double>& y) {
    const auto [res_a, res_b, res_c] = parabolaFitting(x.begin(), x.end(), y.begin());
    std::cout << "Calculated function approximation:\n"
              << res_a << " * x^2 + " << res_b << " * x + " << res_c
              << std::endl;
}

static void adjustValues(std::vector<double>& values) {
    static constexpr double factor_min = 1.0 - parameter::value_adjustment;
    static constexpr double factor_max = 1.0 + parameter::value_adjustment;
    std::uniform_real_distribution<double> dis(factor_min, factor_max);
    std::transform(std::begin(values), std::end(values), std::begin(values),
                   [&dis](double val) { return dis(mersenne) * val; });
}

int main() {
    constexpr double a = 3.14;
    constexpr double b = 2.71;
    constexpr double c = 1.602;

    const auto x_values = getRandomXValues();
    auto y_values = getCalculatedYValues(a, b, c, x_values);
    assert(x_values.size() == y_values.size() && !x_values.empty());

    std::cout << "Result with perfect points" << std::endl;
    fitParabola(x_values, y_values);

    adjustValues(y_values);
    std::cout << "\nResult with inperfect points" << std::endl;
    fitParabola(x_values, y_values);
    return 0;
}
