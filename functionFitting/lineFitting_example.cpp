#include "lineFitting.hpp"

#include <random>

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

static auto getCalculatedYValues(double slope, double y_intersect,
                                 const std::vector<double>& values) {
    std::vector<double> y_values(values.size());
    std::transform(
        std::begin(values), std::end(values), std::begin(y_values),
        [&slope, &y_intersect](double x) { return slope * x + y_intersect; });

    return y_values;
}

static void applySmallError(std::vector<double>& values) {
    static constexpr double factor_min = 1.0 - parameter::value_adjustment;
    static constexpr double factor_max = 1.0 + parameter::value_adjustment;
    std::uniform_real_distribution<double> dis(factor_min, factor_max);
    std::transform(std::begin(values), std::end(values), std::begin(values),
                   [&dis](double val) { return dis(mersenne) * val; });
}

int main() {
    constexpr double slope = 3.14;
    constexpr double y_itersect = 2.71;

    const auto x_values = getRandomXValues();
    auto y_values = getCalculatedYValues(slope, y_itersect, x_values);

    const auto [calculated_slope0, calculated_y_itersect0] =
        lineFitting(x_values.begin(), x_values.end(), y_values.begin());

    std::cout << "Calculated line equation with perfect data:\ny = "
              << calculated_slope0 << " * x + " << calculated_y_itersect0
              << std::endl;

    applySmallError(y_values);
    const auto [calculated_slope1, calculated_y_itersect1] =
        lineFitting(x_values.begin(), x_values.end(), y_values.begin());

    std::cout << "Calculated line equation with inperfect data:\ny = "
              << calculated_slope1 << " * x + " << calculated_y_itersect1
              << std::endl;
    return 0;
}
