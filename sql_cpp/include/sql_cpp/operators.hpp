#pragma once

#include "sql_cpp/detail/operatorbase.hpp"
#include "sql_cpp/detail/table.hpp"

namespace sql_cpp {

namespace detail {

static constexpr inline auto TwoParameterOperator_Offset = 16;
enum class TwoParameterOperatorType : uint8_t {
    // arithmetic types
    ADD = TwoParameterOperator_Offset * uint8_t(OperatorType::arithmetic),
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,

    // bitwise types
    BITWISE_AND = TwoParameterOperator_Offset * uint8_t(OperatorType::bitwise),
    BITWISE_OR,
    BITWISE_XOR,

    // comparison types
    EQUAL_TO = TwoParameterOperator_Offset * uint8_t(OperatorType::comparison),
    GREATER_THAN,
    LESS_THAN,
    GREATER_EQUAL,
    LESS_EQUAL,
    NOT_EQUAL,

    // compound types
    ADD_EQUAL = TwoParameterOperator_Offset * uint8_t(OperatorType::compound),
    SUBTRACT_EQUAL,
    MULTIPLY_EQUAL,
    DIVIDE_EQUAL,
    MODULO_EQUAL,
    BITWISE_AND_EQUAL,
    BITWISE_OR_EQUAL,
    BITWISE_XOR_EQUAL,
};

static constexpr OperatorType
    TwoParameterOperator_to_Operator(TwoParameterOperatorType tpot) {
    return OperatorType(uint8_t(tpot) / TwoParameterOperator_Offset);
}

static constexpr std::string_view
    TwoParameterOperatorType_to_SqlString(TwoParameterOperatorType tpot) {
    // clang-format off
    switch (tpot) {
    // arithmetic types
    case TwoParameterOperatorType::ADD: return "+";
    case TwoParameterOperatorType::SUBTRACT: return "-";
    case TwoParameterOperatorType::MULTIPLY: return "*";
    case TwoParameterOperatorType::DIVIDE: return "/";
    case TwoParameterOperatorType::MODULO: return "%";

    // bitwise types
    case TwoParameterOperatorType::BITWISE_AND: return "&";
    case TwoParameterOperatorType::BITWISE_OR: return "|";
    case TwoParameterOperatorType::BITWISE_XOR: return "^";

    // comparison types
    case TwoParameterOperatorType::EQUAL_TO: return "=";
    case TwoParameterOperatorType::GREATER_EQUAL: return ">=";
    case TwoParameterOperatorType::GREATER_THAN: return ">";
    case TwoParameterOperatorType::LESS_EQUAL: return "<=";
    case TwoParameterOperatorType::LESS_THAN: return "<";
    case TwoParameterOperatorType::NOT_EQUAL: return "<>";

    // compound types
    case TwoParameterOperatorType::ADD_EQUAL: return "&=";
    case TwoParameterOperatorType::SUBTRACT_EQUAL: return "-=";
    case TwoParameterOperatorType::MULTIPLY_EQUAL: return "*=";
    case TwoParameterOperatorType::DIVIDE_EQUAL: return "/=";
    case TwoParameterOperatorType::MODULO_EQUAL: return "%=";
    case TwoParameterOperatorType::BITWISE_AND_EQUAL: return "&=";
    case TwoParameterOperatorType::BITWISE_OR_EQUAL: return "|*=";
    case TwoParameterOperatorType::BITWISE_XOR_EQUAL: return "^-=";
    }
    // clang-format on
    throw std::runtime_error("Invalid Comparison String");
}

} // namespace detail

template <detail::TwoParameterOperatorType Type>
class TwoParameterOperator
    : public detail::SqlOperatorBase<detail::TwoParameterOperator_to_Operator(
          Type)> {
    using super =
        detail::SqlOperatorBase<detail::TwoParameterOperator_to_Operator(Type)>;

  public:
    template <typename Parameter0, typename Parameter1>
    TwoParameterOperator(Parameter0&& lhs, Parameter1&& rhs)
        : m_Comparison(InitOperatorString(std::forward<Parameter0>(lhs),
                                          std::forward<Parameter1>(rhs))),
          m_RequiredClassTypes(
              detail::MultipleTypesToRequiredClassType<Parameter0,
                                                       Parameter1>()) {}

    std::string&& to_string() && noexcept override {
        return std::move(m_Comparison);
    }
    const std::vector<detail::RequiredClassType>&
        requiredClassTypes() const noexcept override {
        return m_RequiredClassTypes;
    }

  private:
    template <typename P0, typename P1>
    static auto InitOperatorString(P0&& p0, P1&& p1) {
        /// TODO: check type checking, compairing string with ints is bad
        return super::TypeToString(std::forward<P0>(p0)) +
               std::string(
                   detail::TwoParameterOperatorType_to_SqlString(Type)) +
               super::TypeToString(std::forward<P1>(p1));
    }

  private:
    std::string m_Comparison;
    const std::vector<detail::RequiredClassType> m_RequiredClassTypes;
};

#define HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(name, type)                  \
    template <typename Parameter0, typename Parameter1>                        \
    auto name(Parameter0&& p0, Parameter1&& p1) {                              \
        return TwoParameterOperator<type>(std::forward<Parameter0>(p0),        \
                                          std::forward<Parameter1>(p1));       \
    }

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    plus, detail::TwoParameterOperatorType::ADD);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    plus_equal, detail::TwoParameterOperatorType::ADD_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    minus, detail::TwoParameterOperatorType::SUBTRACT);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    minus_equal, detail::TwoParameterOperatorType::SUBTRACT_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    multiplies, detail::TwoParameterOperatorType::MULTIPLY);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    multiplies_equal, detail::TwoParameterOperatorType::MULTIPLY_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    divides, detail::TwoParameterOperatorType::DIVIDE);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    divides_equal, detail::TwoParameterOperatorType::DIVIDE_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    modulus, detail::TwoParameterOperatorType::MODULO);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    modulus_equal, detail::TwoParameterOperatorType::MODULO_EQUAL);

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_and, detail::TwoParameterOperatorType::BITWISE_AND);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_and_equal, detail::TwoParameterOperatorType::BITWISE_AND_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_or, detail::TwoParameterOperatorType::BITWISE_OR);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_or_equal, detail::TwoParameterOperatorType::BITWISE_OR_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_xor, detail::TwoParameterOperatorType::BITWISE_XOR);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_xor_equal, detail::TwoParameterOperatorType::BITWISE_XOR_EQUAL);

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    equal_to, detail::TwoParameterOperatorType::EQUAL_TO);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    greater, detail::TwoParameterOperatorType::GREATER_THAN);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    greater_equal, detail::TwoParameterOperatorType::GREATER_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    less, detail::TwoParameterOperatorType::LESS_THAN);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    less_equal, detail::TwoParameterOperatorType::LESS_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    not_equal_to, detail::TwoParameterOperatorType::NOT_EQUAL);

#undef HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION

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

template <typename Value, typename LowerTarget, typename UpperTarget>
auto between(Value&& value, LowerTarget&& lower, UpperTarget&& upper) {
    return OperatorBetween(std::forward<Value>(value),
                           std::forward<LowerTarget>(lower),
                           std::forward<UpperTarget>(upper));
}

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
        std::string&& frontStr = detail::TableBase<int>::GetSqlValueString(front);
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

template <typename Value, typename... Args>
auto in(Value&& value, Args&&... args) {
    return OperatorIn(std::forward<Value>(value), std::forward<Args>(args)...);
}

} // namespace sql_cpp
