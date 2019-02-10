#pragma once

// equations based on: https://mste.illinois.edu/regression/directory.html

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>

// alternative iterator implementation
template <typename _InputIter0, typename _InputIter1>
decltype(auto) lineFitting(_InputIter0 begin0, _InputIter0 end0,
                           _InputIter1 begin1) {
    using _ResType = decltype((*begin0) * (*begin1));
    namespace ba = boost::accumulators;

    ba::accumulator_set<
        _ResType, ba::stats<ba::tag::variance,
                            ba::tag::covariance<_ResType, ba::tag::covariate1>>>
        stat_x;

    ba::accumulator_set<_ResType, ba::stats<ba::tag::mean>> stat_y;

    std::for_each(begin0, end0, [&begin1, &stat_x, &stat_y](const _ResType& x) {
        stat_y(*begin1);
        stat_x(x, ba::covariate1 = *begin1++);
    });
    const auto slope = ba::covariance(stat_x) / ba::variance(stat_x);
    return std::make_pair(slope, ba::mean(stat_y) - slope * ba::mean(stat_x));
}
