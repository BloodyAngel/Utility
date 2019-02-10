#pragma once

#include <algorithm>
#include <numeric>
#include <tuple>

namespace detail {
    template <typename Type> struct _BaseParameter {
        size_t numDataPoints = 0;

        Type sum_t1 = 0.0;
        Type sum_t2 = 0.0;
        Type sum_t3 = 0.0;
        Type sum_t4 = 0.0;

        Type sum_dt0 = 0.0;
        Type sum_dt1 = 0.0;
        Type sum_dt2 = 0.0;
    };

    template <typename _ResultType, typename _InputIter0, typename _InputIter1>
    decltype(auto) _calculateBaseParameter(_InputIter0 begin0, _InputIter0 end0,
                                           _InputIter1 begin1) {
        // calculate vector_d (right hand side)
        // and
        // calculate stum(t), sum(t^2), sum(t^3) and sum(t^4)
        _BaseParameter<_ResultType> parameter;
        std::for_each(begin0, end0, [&parameter, &begin1](_ResultType value) {
            ++parameter.numDataPoints;
            const _ResultType temp_y = *begin1++;
            const _ResultType temp_x = value;
            const _ResultType temp_x_sq = temp_x * temp_x;

            // calculate right handside (vector_d)
            parameter.sum_dt0 += temp_y;
            parameter.sum_dt1 += temp_y * temp_x;
            parameter.sum_dt2 += temp_y * temp_x_sq;

            // calculate matrix values
            parameter.sum_t1 += temp_x;
            parameter.sum_t2 += temp_x_sq;
            parameter.sum_t3 += temp_x_sq * temp_x;
            parameter.sum_t4 += temp_x_sq * temp_x_sq;
        });
        return parameter;
    }
} // namespace detail

// alternative iterator implementation
template <typename _InputIter0, typename _InputIter1>
decltype(auto) parabolaFitting(_InputIter0 begin0, _InputIter0 end0,
                               _InputIter1 begin1) {

    using _ResultType = decltype((*begin0) * (*begin1));

    // create parabalo function based on data
    // link to algorithm: http://www.personal.psu.edu/jhm/f90/lectures/lsq2.html
    // possible implementation (pastebin.com -> license?)
    // http://pastebin.com/tUvKmGPn
    //
    // own implementation, differs from pastebin in last step
    const auto bp =
        detail::_calculateBaseParameter<_ResultType>(begin0, end0, begin1);

    // clang-format off

/* implementation without substitution
    const T a = (sum_dt2 - sum_t2 * sum_dt0 / dataSize - (sum_dt1 * dataSize - sum_t1 * sum_dt0) / (sum_t2 * dataSize - sum_t1 * sum_t1) * (sum_t3 - sum_t1 * sum_t2 / dataSize))
        /
        ((sum_t1 * sum_t2 - sum_t3 * dataSize) / (sum_t2 * dataSize - sum_t1 * sum_t1) * (sum_t3 - sum_t1 * sum_t2 / dataSize) + sum_t4 - sum_t2 * sum_t2 / dataSize);


    const T b = (sum_dt1 * dataSize - sum_t1 * sum_dt0 + a * (sum_t1 * sum_t2 - sum_t3 * dataSize) ) / (sum_t2 * dataSize - sum_t1 * sum_t1);
    const T c = (sum_dt0 - sum_t1 * b - sum_t2 * a) / dataSize;
*/

    /*
    algorithm found at:
    http://www.personal.psu.edu/jhm/f90/lectures/lsq2.html
    http://stackoverflow.com/questions/11233404/simple-curve-fitting-implimentation-in-c-svd-least-sqares-fit-or-similar

    different approach found at:
    http://pastebin.com/tUvKmGPn


    equations to solve:
    n  *  c  + t1 *  b  + t2 *  a   = d0        <=>     c = (d0 - t1 * b - t2 * a) / n  I
    t1 *  c  + t2 *  b  + t3 *  a   = d1    II
    t2 *  c  + t3 *  b  + t4 *  a   = d2    III



    I in II:
    --------

    t1 * (d0 - t1 * b - t2 * a) / n + t2 * b + t3 * a = d1


            -t1^2 * b                     t1 * d0        t1 * t2 * a
    <=>     ----------  + t2 * b = d1 -  ---------  +   -------------    + t2 * a
                n                            n                n


                    d1 * n - t1 * d0                 t1 * t2 - t3 * n
    <=>     b =    ------------------   +   a *     ------------------              IV
                     t2 * n - t1^2                    t2 * n - t1^2




    I in III:
    ---------

    t1 * (d0 - t1 * b - t2 * a / n + t3 * b + t4 * a = d2


             t2 * d0         t1 * t2                 t2^2
    <=>     ---------   -   ----------  * b     -   -------  * a    + t3 * b  + t4 * a = d2
                n               n                       n


                 /           t2^2   \            /           t2^2   \                t2 * d0
    <=>     b *  |  t3  -   ------  |   +   a *  |   t4 -   ------  |   =   d2 -    ----------          V
                 \            n     /            \             n    /                   n




    IV in V:
    --------

        /    d1 * n - t1 * 0            t1 * t2 - t3 * n    \       /            t1 * t2    \            /           t2^2    \
        |   -----------------   + a *  ------------------   |   *   |   t3  -   ---------   |    +   a * |  t4  -   ------   |
        \     t2 * n - t1^2             t2 * n - t1^2       /       \               n       /            \            n      /

                t2 * d0          /    d1 * n -t1 * d0    \       /            t1 * t2    \
    =   d2 -   ---------     -   |   -----------------   |   *   |   t3 -    ---------   |
                   n             \     t2 * n - t1^2     /       \               n       /




                /   /  t1 * t2 - t3 * n   \       /          t1 * t2    \       /            t2^2   \
    <=>     a * |   | ------------------  |   *   |   t3 -  ---------   |   +   |   t4  -   ------  |
                \   \   t2 * n - t1^2     /       \             n       /       \             n     /


                     t2 * d0        /   d1 * n - t1 * d0    \       /            t1 * t2    \
        =   d2  -   ---------   -   |  ------------------   |   *   |   t4  -   ----------  |
                        n           \    t2 * n - t1^2      /       \               n       /



    TODO last step



    variable description:

    a, b and c are the parabola coefficients:
    f(x) = a * x^2 + b * x + c

    n   = number of data points                     (dataSize   in this implementation)

    t1  = sum of x  ,    values from 0 to size-1    (sum_t1     in this implementation)
    t2  = sum of x^2,    values from 0 to size-1    (sum_t2     in this implementation)
    t3  = sum of x^3,    values from 0 to size-1    (sum_t3     in this implementation)
    t4  = sum of x^4,    values from 0 to size-1    (sum_t4     in this implementation)

    dt0 = sum of y*x^0,  values from 0 to size-1    (sum_dt0    in this implementation)
    dt1 = sum of y*x^1,  values from 0 to size-1    (sum_dt1    in this implementation)
    dt2 = sum of y*x^2,  values from 0 to size-1    (sum_dt2    in this implementation)
    */

    const _ResultType dataSize(bp.numDataPoints);
    const _ResultType substition_1 = bp.sum_t2 * dataSize - bp.sum_t1 * bp.sum_t1;
    const _ResultType substition_2 = bp.sum_t3 - bp.sum_t1 * bp.sum_t2 / dataSize;

    const _ResultType a = (bp.sum_dt2 - bp.sum_t2 * bp.sum_dt0 / dataSize - ((bp.sum_dt1 * dataSize - bp.sum_t1 * bp.sum_dt0) / substition_1) * substition_2) /
        (((bp.sum_t1 * bp.sum_t2 - bp.sum_t3 * dataSize) / substition_1) * substition_2 + bp.sum_t4 - bp.sum_t2 * bp.sum_t2 / dataSize);

    const _ResultType b = (bp.sum_dt1 * dataSize - bp.sum_t1 * bp.sum_dt0 + a * (bp.sum_t1 * bp.sum_t2 - bp.sum_t3 * dataSize)) / substition_1;

    const _ResultType c = (bp.sum_dt0 - bp.sum_t1 * b - bp.sum_t2 * a) / dataSize;

    // clang-format on

    return std::make_tuple(a, b, c);
}
