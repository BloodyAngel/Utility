#pragma once

#include "sql_cpp/detail/operatorbase.hpp"
#include "sql_cpp/detail/table.hpp"

namespace sql_cpp::detail {

static constexpr inline auto NonLogicalOperator_Offset = 16;
enum class NonLogicalOperatorType : uint8_t {
    // arithmetic types
    ADD = NonLogicalOperator_Offset * uint8_t(OperatorType::arithmetic),
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,

    // bitwise types
    BITWISE_AND = NonLogicalOperator_Offset * uint8_t(OperatorType::bitwise),
    BITWISE_OR,
    BITWISE_XOR,

    // comparison types
    EQUAL_TO = NonLogicalOperator_Offset * uint8_t(OperatorType::comparison),
    GREATER_THAN,
    LESS_THAN,
    GREATER_EQUAL,
    LESS_EQUAL,
    NOT_EQUAL,

    // compound types
    ADD_EQUAL = NonLogicalOperator_Offset * uint8_t(OperatorType::compound),
    SUBTRACT_EQUAL,
    MULTIPLY_EQUAL,
    DIVIDE_EQUAL,
    MODULO_EQUAL,
    BITWISE_AND_EQUAL,
    BITWISE_OR_EQUAL,
    BITWISE_XOR_EQUAL,
};

static constexpr OperatorType
    NonLogicalOperator_to_Operator(NonLogicalOperatorType tpot) {
    return OperatorType(uint8_t(tpot) / NonLogicalOperator_Offset);
}

static constexpr std::string_view
    NonLogicalOperatorType_to_SqlString(NonLogicalOperatorType tpot) {
    // clang-format off
    switch (tpot) {
    // arithmetic types
    case NonLogicalOperatorType::ADD: return "+";
    case NonLogicalOperatorType::SUBTRACT: return "-";
    case NonLogicalOperatorType::MULTIPLY: return "*";
    case NonLogicalOperatorType::DIVIDE: return "/";
    case NonLogicalOperatorType::MODULO: return "%";

    // bitwise types
    case NonLogicalOperatorType::BITWISE_AND: return "&";
    case NonLogicalOperatorType::BITWISE_OR: return "|";
    case NonLogicalOperatorType::BITWISE_XOR: return "^";

    // comparison types
    case NonLogicalOperatorType::EQUAL_TO: return "=";
    case NonLogicalOperatorType::GREATER_EQUAL: return ">=";
    case NonLogicalOperatorType::GREATER_THAN: return ">";
    case NonLogicalOperatorType::LESS_EQUAL: return "<=";
    case NonLogicalOperatorType::LESS_THAN: return "<";
    case NonLogicalOperatorType::NOT_EQUAL: return "<>";

    // compound types
    case NonLogicalOperatorType::ADD_EQUAL: return "&=";
    case NonLogicalOperatorType::SUBTRACT_EQUAL: return "-=";
    case NonLogicalOperatorType::MULTIPLY_EQUAL: return "*=";
    case NonLogicalOperatorType::DIVIDE_EQUAL: return "/=";
    case NonLogicalOperatorType::MODULO_EQUAL: return "%=";
    case NonLogicalOperatorType::BITWISE_AND_EQUAL: return "&=";
    case NonLogicalOperatorType::BITWISE_OR_EQUAL: return "|*=";
    case NonLogicalOperatorType::BITWISE_XOR_EQUAL: return "^-=";
    }
    // clang-format on
    throw std::runtime_error("Invalid Comparison String");
}

template <detail::NonLogicalOperatorType Type>
class NonLogicalOperator
    : public detail::SqlOperatorBase<detail::NonLogicalOperator_to_Operator(
          Type)> {
    using super =
        detail::SqlOperatorBase<detail::NonLogicalOperator_to_Operator(Type)>;

  public:
    template <typename Parameter0, typename Parameter1>
    NonLogicalOperator(Parameter0&& lhs, Parameter1&& rhs)
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
               std::string(detail::NonLogicalOperatorType_to_SqlString(Type)) +
               super::TypeToString(std::forward<P1>(p1));
    }

  private:
    std::string m_Comparison;
    const std::vector<detail::RequiredClassType> m_RequiredClassTypes;
};

} // namespace sql_cpp::detail

namespace sql_cpp {

#define HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(name, type)                  \
    template <typename Parameter0, typename Parameter1>                        \
    auto name(Parameter0&& p0, Parameter1&& p1) {                              \
        return detail::NonLogicalOperator<type>(std::forward<Parameter0>(p0),  \
                                                std::forward<Parameter1>(p1)); \
    }

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(plus,
                                          detail::NonLogicalOperatorType::ADD);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    plus_equal, detail::NonLogicalOperatorType::ADD_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    minus, detail::NonLogicalOperatorType::SUBTRACT);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    minus_equal, detail::NonLogicalOperatorType::SUBTRACT_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    multiplies, detail::NonLogicalOperatorType::MULTIPLY);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    multiplies_equal, detail::NonLogicalOperatorType::MULTIPLY_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    divides, detail::NonLogicalOperatorType::DIVIDE);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    divides_equal, detail::NonLogicalOperatorType::DIVIDE_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    modulus, detail::NonLogicalOperatorType::MODULO);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    modulus_equal, detail::NonLogicalOperatorType::MODULO_EQUAL);

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_and, detail::NonLogicalOperatorType::BITWISE_AND);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_and_equal, detail::NonLogicalOperatorType::BITWISE_AND_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_or, detail::NonLogicalOperatorType::BITWISE_OR);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_or_equal, detail::NonLogicalOperatorType::BITWISE_OR_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_xor, detail::NonLogicalOperatorType::BITWISE_XOR);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    bit_xor_equal, detail::NonLogicalOperatorType::BITWISE_XOR_EQUAL);

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    equal_to, detail::NonLogicalOperatorType::EQUAL_TO);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    greater, detail::NonLogicalOperatorType::GREATER_THAN);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    greater_equal, detail::NonLogicalOperatorType::GREATER_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    less, detail::NonLogicalOperatorType::LESS_THAN);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    less_equal, detail::NonLogicalOperatorType::LESS_EQUAL);
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(
    not_equal_to, detail::NonLogicalOperatorType::NOT_EQUAL);

#undef HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION

namespace detail {

template <typename ValueType, typename TableStruct> class OperatorOverloadBase {
  public:
    constexpr OperatorOverloadBase(ValueType TableStruct::*ptr) : m_Ptr(ptr) {}

  protected:
    const ValueType TableStruct::*m_Ptr;
};

#define HELPER_MACRO_CREATE_OPERATOR_MEMBER(Operator, funcName)                \
    auto operator Operator(ValueType&& rhs) const {                            \
        return funcName(this->m_Ptr, std::move(rhs));                          \
    }                                                                          \
    auto operator Operator(const ValueType& rhs) const {                       \
        return funcName(this->m_Ptr, rhs);                                     \
    }                                                                          \
    friend auto operator Operator(ValueType&& lhs, const self_type& self) {    \
        return funcName(std::move(lhs), self.m_Ptr);                           \
    }                                                                          \
    friend auto operator Operator(const ValueType& lhs,                        \
                                  const self_type& self) {                     \
        return funcName(lhs, self.m_Ptr);                                      \
    }

template <typename ValueType, typename TableStruct>
class Comparison : public OperatorOverloadBase<ValueType, TableStruct> {
  public:
    using self_type = Comparison;
    constexpr Comparison(ValueType TableStruct::*ptr)
        : OperatorOverloadBase<ValueType, TableStruct>(ptr) {}

    HELPER_MACRO_CREATE_OPERATOR_MEMBER(==, equal_to)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(!=, not_equal_to)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(<, less)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(<=, less_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(>, greater)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(>=, greater_equal)
};

template <typename ValueType, typename TableStruct>
class Arithmetic : public OperatorOverloadBase<ValueType, TableStruct> {
  public:
    using self_type = Arithmetic;
    constexpr Arithmetic(ValueType TableStruct::*ptr)
        : OperatorOverloadBase<ValueType, TableStruct>(ptr) {}

    HELPER_MACRO_CREATE_OPERATOR_MEMBER(+, plus)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(-, minus)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(*, multiplies)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(/, divides)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(%, modulus)
};

template <typename ValueType, typename TableStruct>
class Bitwise : public OperatorOverloadBase<ValueType, TableStruct> {
  public:
    using self_type = Bitwise;
    constexpr Bitwise(ValueType TableStruct::*ptr)
        : OperatorOverloadBase<ValueType, TableStruct>(ptr) {}

    HELPER_MACRO_CREATE_OPERATOR_MEMBER(&, bit_and)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(|, bit_or)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(^, bit_xor)
};

template <typename ValueType, typename TableStruct>
class Compound : public OperatorOverloadBase<ValueType, TableStruct> {
  public:
    using self_type = Compound;
    constexpr Compound(ValueType TableStruct::*ptr)
        : OperatorOverloadBase<ValueType, TableStruct>(ptr) {}

    HELPER_MACRO_CREATE_OPERATOR_MEMBER(+=, plus_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(-=, minus_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(*=, multiplies_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(/=, divides_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(%=, modulus_equal)

    HELPER_MACRO_CREATE_OPERATOR_MEMBER(&=, bit_and_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(|=, bit_or_equal)
    HELPER_MACRO_CREATE_OPERATOR_MEMBER(^=, bit_xor_equal)
};

#undef HELPER_MACRO_CREATE_OPERATOR_MEMBER

/**
 * 'public virtual' seem to have bigger size than just raw inheritance
 */
template <typename ValueType, typename TableStruct>
class AllOperators : public Comparison<ValueType, TableStruct>,
                     public Arithmetic<ValueType, TableStruct>,
                     public Bitwise<ValueType, TableStruct>,
                     public Compound<ValueType, TableStruct> {
  public:
    constexpr AllOperators(ValueType TableStruct::*ptr)
        : Comparison<ValueType, TableStruct>(ptr),
          Arithmetic<ValueType, TableStruct>(ptr),
          Bitwise<ValueType, TableStruct>(ptr),
          Compound<ValueType, TableStruct>(ptr) {}
};

} // namespace detail

#define HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(funcName, ClassName)         \
    template <typename ValueType, typename TableStruct>                        \
    auto funcName(ValueType TableStruct::*ptr) {                               \
        return detail::ClassName<ValueType, TableStruct>(ptr);                 \
    }

HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(arithmetic, Arithmetic)
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(bitwise, Bitwise)
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(comparison, Comparison)
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(compound, Compound)
HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION(operators, AllOperators)

#undef HELPER_MACRO_CREATE_SQL_OPERATOR_FUNCTION

} // namespace sql_cpp
