#pragma once

#include "sql_cpp/detail/operatorbase.hpp"
#include "sql_cpp/detail/table.hpp"

namespace sql_cpp::detail {

class OperatorBetween
    : public detail::SqlOperatorBase<detail::OperatorType::logical> {

    using super = detail::SqlOperatorBase<detail::OperatorType::logical>;

  public:
    template <typename Value, typename LowerTarget, typename UpperTarget>
    OperatorBetween(Value&& value, LowerTarget&& lower, UpperTarget&& upper)
        : m_Comparison(InitOperatorString(std::forward<Value>(value),
                                          std::forward<LowerTarget>(lower),
                                          std::forward<UpperTarget>(upper))),
          m_RequiredClassType(
              detail::MultipleTypesToRequiredClassType<Value, LowerTarget,
                                                       UpperTarget>()) {}

    std::string&& to_string() && noexcept override {
        return std::move(m_Comparison);
    }
    const std::vector<detail::RequiredClassType>&
        requiredClassTypes() const noexcept override {
        return m_RequiredClassType;
    }

  private:
    template <typename Value, typename LowerTarget, typename UpperTarget>
    static auto InitOperatorString(Value&& val, LowerTarget&& lower,
                                   UpperTarget&& upper) {

        /// TODO: check type checking, compairing string with ints is bad
        auto&& valStr = super::TypeToString(std::forward<Value>(val));
        auto&& lowerStr = super::TypeToString(std::forward<LowerTarget>(lower));
        auto&& upperStr = super::TypeToString(std::forward<UpperTarget>(upper));

        return std::move(valStr) + " BETWEEN " + std::move(lowerStr) + " AND " +
               std::move(upperStr);
    }
    std::string m_Comparison;
    const std::vector<detail::RequiredClassType> m_RequiredClassType;
};

class OperatorIn
    : public detail::SqlOperatorBase<detail::OperatorType::logical> {

    using super = detail::SqlOperatorBase<detail::OperatorType::logical>;

  public:
    template <typename Value, typename... Args>
    OperatorIn(Value&& value, Args&&... args)
        : m_Comparison(InitOperatorString(std::forward<Value>(value),
                                          std::forward<Args>(args)...)),
          m_RequiredClassType(
              detail::MultipleTypesToRequiredClassType<Value, Args...>()) {}

    std::string&& to_string() && noexcept override {
        return std::move(m_Comparison);
    }
    const std::vector<detail::RequiredClassType>&
        requiredClassTypes() const noexcept override {
        return m_RequiredClassType;
    }

  private:
    template <typename Front, typename... Args>
    static auto InitOperatorString_Args(Front&& front, Args&&... args) {
        std::string&& frontStr =
            detail::TableBase<int>::GetSqlValueString(front);
        if constexpr (sizeof...(Args))
            return std::move(frontStr) + ',' + InitOperatorString_Args(args...);
        else
            return std::move(frontStr);
    }

    template <typename Value, typename... Args>
    static auto InitOperatorString(Value&& value, Args&&... args) {

        /// TODO: check type checking, compairing string with ints is bad
        return super::TypeToString(std::forward<Value>(value)) + " IN (" +
               InitOperatorString_Args(args...) + ")";
    }
    std::string m_Comparison;
    const std::vector<detail::RequiredClassType> m_RequiredClassType;
};

} // namespace sql_cpp::detail

namespace sql_cpp {

template <typename Value, typename LowerTarget, typename UpperTarget>
auto between(Value&& value, LowerTarget&& lower, UpperTarget&& upper) {
    return detail::OperatorBetween(std::forward<Value>(value),
                                   std::forward<LowerTarget>(lower),
                                   std::forward<UpperTarget>(upper));
}

template <typename Value, typename... Args>
auto in(Value&& value, Args&&... args) {
    return detail::OperatorIn(std::forward<Value>(value),
                              std::forward<Args>(args)...);
}

} // namespace sql_cpp
