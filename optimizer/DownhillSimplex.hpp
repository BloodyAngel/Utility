#pragma once
#ifndef DOWNHILL_SIMPLEX_HPP_HPP
#define DOWNHILL_SIMPLEX_HPP_HPP

#include <cmath>
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <boost/config.hpp>

template<typename _ObjectiveFunction, typename _ContainerType,
         typename _DurationValueType = std::chrono::hours::rep,
         typename _DurationRatio = std::chrono::hours::period>
_ContainerType downhill_simplex(const _ObjectiveFunction& eval, std::vector<_ContainerType> guess,
                         const typename _ContainerType::value_type tolerance = 0.0,
                         const size_t maxIteration = std::numeric_limits<size_t>::max(),
                         const std::chrono::duration<_DurationValueType, _DurationRatio>
                                maxDuration = std::chrono::hours(24 * 365))
{
    using namespace std::chrono;
    using vt = typename _ContainerType::value_type;
    if (guess.empty())
        return {};

    const system_clock::time_point start = system_clock::now();
    const size_t numDimensions = guess.front().size();
    if (guess.size() < numDimensions + 1){
        // initialize simplex points
        _ContainerType avg(numDimensions, 0.0);
        for (const auto& e : guess)
            std::transform(e.cbegin(), e.cend(), avg.cbegin(), avg.begin(), std::plus<vt>());

        std::transform(avg.cbegin(), avg.cend(), avg.begin(), [&guess](const vt& v) -> vt{ return v / guess.size(); });

        guess.reserve(numDimensions - guess.size() + 1);
        for (size_t i = 0; guess.size() < numDimensions + 1; ++i){
            // add adjusted value to guess
            // adjustment: 1 parameter will get +10%
            guess.push_back(avg);
            auto iter = guess.back().begin();
            std::advance(iter, i);
            (*iter) *= 1.10;
        }
    }
    // Paramter
    constexpr const double para_reflect = 1.0, para_expand = 1.0, para_contract = 0.5;

    std::vector<vt> y_currentSimplex;
    y_currentSimplex.reserve(guess.size());
    for (const auto& e : guess)
        y_currentSimplex.push_back(eval(e));

    _ContainerType x_centroid(numDimensions);
    _ContainerType x_expanded(numDimensions);
    _ContainerType x_reflected(numDimensions);
    _ContainerType x_contracted(numDimensions);
    size_t idx_min = std::numeric_limits<size_t>::max();
    size_t idx_max = std::numeric_limits<size_t>::max();

    size_t varianceCounter = 0;
    auto optimizationFinish = [&guess, &varianceCounter, &y_currentSimplex, &tolerance, &maxDuration, &start]() -> bool{
        // time constraint
        if (BOOST_UNLIKELY(system_clock::now() - start > maxDuration))
            return true;

        // tolerance constraint, based on y- or x-value varinace
#ifndef DOWNHILL_SIMPLEX_Y_VLAUE_VARIATION
        double coefVariance = 0.0;
        for (const auto& data : guess){
#else
            const auto& data = y_currentSimplex;
#endif
            double mean = 0.0;
            for (const auto& e : data)
                mean += e;
            mean /= double(data.size());

            double variance = 0.0;
            for (const auto& e : data){
                const double tmp = e - mean;
                variance += tmp * tmp;
            }
#ifndef DOWNHILL_SIMPLEX_Y_VLAUE_VARIATION
            coefVariance += std::sqrt(variance) / std::abs(mean);
        }
        coefVariance /= double(guess.size());
#else
        const double coefVariance = std::sqrt(variance) / mean;
#endif

        if (BOOST_UNLIKELY(coefVariance <= tolerance)){
            ++varianceCounter;
            if (varianceCounter > 3)
                return true;
        }
        else
            varianceCounter = 0;

        return false;
    };

    auto accept = [&idx_max, &guess, &y_currentSimplex](const _ContainerType& x_value, const vt& y_value){
        y_currentSimplex[idx_max] = y_value;
        guess[idx_max] = x_value;
    };
    size_t iterationCounter = 0;
    for (; iterationCounter < maxIteration; ++iterationCounter){
        size_t idx_2ndMax;
        idx_min = 0;
        idx_max = 0;

        // find min, max and 2ndMax
        for (size_t i = 1; i < y_currentSimplex.size(); ++i){
            if (y_currentSimplex[idx_min] > y_currentSimplex[i])
                idx_min = i;
            else if (y_currentSimplex[idx_max] < y_currentSimplex[i])
                idx_max = i;
        }

        idx_2ndMax = idx_min;
        for (size_t i = 1; i < y_currentSimplex.size(); ++i){
            if (y_currentSimplex[idx_2ndMax] < y_currentSimplex[i] && y_currentSimplex[i] < y_currentSimplex[idx_max])
                idx_2ndMax = i;
        }

        // calculate centroid
        std::fill(x_centroid.begin(), x_centroid.end(), vt(0.0));
        for (const auto& e : guess)
            std::transform(e.cbegin(), e.cend(), x_centroid.cbegin(), x_centroid.begin(), std::plus<vt>());

        std::transform(x_centroid.cbegin(), x_centroid.cend(), guess[idx_max].cbegin(), x_centroid.begin(), std::minus<vt>());
        std::transform(x_centroid.cbegin(), x_centroid.cend(), x_centroid.begin(), [&numDimensions](const vt& v) -> vt{
            return v / vt(numDimensions);
        });

        // reflection
        std::transform(x_centroid.cbegin(), x_centroid.cend(), guess[idx_max].cbegin(), x_reflected.begin(),
                       [](const vt& v_centroid, const vt& v_guess) -> vt{
            return (1.0 + para_reflect) * v_centroid - para_reflect * v_guess;
        });

        /// TODO
        /// change to c++17 if with initializer statements
        const vt y_reflected = eval(x_reflected);
        if (y_reflected < y_currentSimplex[idx_min]){ // expansion
            std::transform(x_centroid.cbegin(), x_centroid.cend(), x_reflected.cbegin(), x_expanded.begin(),
                           [](const vt& v_centroid, const vt& v_reflected) -> vt{
                return (1.0 + para_expand) * v_reflected - para_expand * v_centroid;
            });

            /// TODO
            /// change to c++17 if with initializer statements
            const vt y_expanded = eval(x_expanded);
            if (y_expanded < y_currentSimplex[idx_min])
            //if (y_expanded < y_reflected) // IGD version uses this if
                accept(x_expanded, y_expanded);
            else
                accept(x_reflected, y_reflected);
        }
        else if (y_reflected <= y_currentSimplex[idx_2ndMax])
            accept(x_reflected, y_reflected);
        else { // contraction
            if (y_reflected < y_currentSimplex[idx_max])
                accept(x_reflected, y_reflected);

            std::transform(x_centroid.cbegin(), x_centroid.cend(), guess[idx_max].cbegin(), x_contracted.begin(),
                           [](const vt& v_centroid, const vt& v_guess) -> vt{
                return para_contract * v_guess + (1.0 - para_contract) * v_centroid;
            });

            /// TODO
            /// change to c++17 if with initializer statements
            const vt y_contracted = eval(x_contracted);
            if (y_contracted < y_currentSimplex[idx_max])
                accept(x_contracted, y_contracted);
            else{
                // shrink simplex
                for (size_t i = 0; i < guess.size(); ++i){
                    if (BOOST_UNLIKELY(i == idx_min))
                        continue;

                    std::transform(guess[i].cbegin(), guess[i].cend(), guess[idx_min].cbegin(), guess[i].begin(),
                                   [](const vt& lhs, const vt& rhs) -> vt{ return (lhs + rhs) * 0.5; });

                    y_currentSimplex[i] = eval(guess[i]);
                }
            }
        }
        if (optimizationFinish())
            break;
    }
    idx_min = 0;
    for (size_t i = 1; i < y_currentSimplex.size(); ++i){
        if (y_currentSimplex[i] < y_currentSimplex[idx_min])
            idx_min = i;
    }
    double requiredTime = double(duration_cast<microseconds>(system_clock::now() - start).count()) / 1000.0;
    std::string timeExtension = "ms";

#define TMP_TIME_RATIO(nextRatio, nextExtension)                               \
    if (requiredTime > nextRatio) {                                            \
        requiredTime /= nextRatio;                                             \
        timeExtension = nextExtension;

    TMP_TIME_RATIO(1000.0, "s")
        TMP_TIME_RATIO(60.0, "min")
            TMP_TIME_RATIO(60.0, "h")
                TMP_TIME_RATIO(24.0, "days")
                    TMP_TIME_RATIO(7.0, "weeks")
    }   }   }   }   }
#undef TMP_TIME_RATIO

    std::cout << "\nDownhillsimplex finished!\nrequired iterations:  " << iterationCounter
              << "\nrequired   time    :  " << double(int(requiredTime * 100) / 100.0) << ' '
              << timeExtension << '\n' << std::endl;
    return guess.at(idx_min);
}


#endif // DOWNHILL_SIMPLEX_HPP_HPP

