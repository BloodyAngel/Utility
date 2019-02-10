#include "DownhillSimplex.hpp"

#include <assert.h>
#include <numeric>

namespace parabola {
    static constexpr auto a = 3.14;
    static constexpr auto b = 2.71;
    static constexpr auto c = 1.602;

    double equation(const std::vector<double>& v) {
        const double& val = v.front();
        return a * val * val + b * val + c;
    }
    double getPerfectXPosition() { return -b / (2 * a); }
} // namespace parabola

namespace higherOrderFunction {
    // x^6 - x^5 - x^4 - x^3 + x^2 + x - 1
    // link to see a plot of the function:
    // https://www.wolframalpha.com/input/?i=x%5E6+-+x%5E5+-+x%5E4+-+x%5E3+%2B+x%5E2+%2B+x+-+1
    static constexpr std::array parameters = {1.0, -1.0, -1.0, -1.0, 1.0, 1.0};

    double equation(const std::vector<double>& v) {
        assert(v.size() == 1);
        auto exponent = parameters.size();
        const double value = v.front();
        return std::accumulate(
            parameters.begin(), parameters.end(), 0.0,
            [&exponent, value](double sum, double parameter) {
                return sum + std::pow(value, double(exponent--)) * parameter;
            });
    }
} // namespace higherOrderFunction

void parabolaExample() {
    const std::vector<std::vector<double>> guess = {{1000.0}};
    const auto result = downhill_simplex(parabola::equation, guess);
    assert(result.size() == guess.front().size());

    std::cout << "Parabola result:\t\t" << result.front()
              << "\nOptimal  result:\t\t" << parabola::getPerfectXPosition()
              << std::endl;
}

void higherOrderFunctionExample() {
    auto calc = [](const std::vector<std::vector<double>>& guess) {
        const auto result =
            downhill_simplex(higherOrderFunction::equation, guess);
        assert(result.size() == guess.front().size());

        std::cout << "Higher order function result:\t\t" << result.front()
                  << std::endl;
    };

    // high distance guess
    std::cout << "High distance guess:\n";
    calc({{-1000.0}});

    // guess near local minima
    std::cout << "Guess near local minma:\n";
    calc({{-0.1}});
}

int main() {
    parabolaExample();
    higherOrderFunctionExample();
    return 0;
}
