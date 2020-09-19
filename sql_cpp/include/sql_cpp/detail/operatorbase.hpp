#pragma once

#include <stdexcept>
#include <string>

#include "sql_cpp/detail/table.hpp"
#include "sql_cpp/detail/util.hpp"

namespace sql_cpp::detail {

enum class OperatorType : uint8_t {
    arithmetic,
    bitwise,
    comparison,
    compound,
    logical
};

struct RequiredClassType {
    size_t hash = 0;
    std::string className;
};

template <typename T, typename ValueType>
static T TypeToRequiredClassType_ExtractClassHelper(ValueType T::*);

template <typename Type> static RequiredClassType TypeToRequiredClassType() {
    RequiredClassType result;
    if constexpr (std::is_member_object_pointer_v<Type>) {
        using ClassType =
            decltype(TypeToRequiredClassType_ExtractClassHelper(Type()));

        result.hash = typeid(ClassType).hash_code();
        result.className = GetClassNameStringView<ClassType>();
    }
    return result;
}

template <typename FirstParameterType, typename... OtherParameterTypes>
static std::vector<RequiredClassType> MultipleTypesToRequiredClassType() {
    std::vector<RequiredClassType> result;
    result.push_back(TypeToRequiredClassType<FirstParameterType>());

    if constexpr (sizeof...(OtherParameterTypes)) {
        auto&& recursiveResult =
            MultipleTypesToRequiredClassType<OtherParameterTypes...>();

        result.insert(result.end(), recursiveResult.cbegin(),
                      recursiveResult.cend());
    }
    return result;
}

template <OperatorType TypeOfOperator> struct SqlOperatorBase {
    consteval OperatorType getOperatorType() noexcept { return TypeOfOperator; }

    virtual const std::vector<RequiredClassType>&
        requiredClassTypes() const noexcept = 0;

    virtual std::string&& to_string() && noexcept = 0;

    template <typename T> static std::string TypeToString(T&& value) {
        using RawType = std::decay_t<T>;

        /// TODO: maybe add support for wstring
        if constexpr (std::is_member_object_pointer_v<RawType>) {
            using ClassType =
                decltype(TypeToRequiredClassType_ExtractClassHelper(value));

            using TCT = Table<ClassType>;
            return TCT::GetTableName().to_string() + '.' +
                   TCT::GetColumenName(value).to_string();

        } else if constexpr (std::is_arithmetic_v<RawType>)
            return std::to_string(std::forward<T>(value));
        else if constexpr (std::is_convertible_v<RawType, std::string>)
            return std::forward<T>(value);
        else
            throw std::runtime_error("Invalid Parametertype");
    }
};

class ComparisonSqlOperatorHelper
    : public SqlOperatorBase<OperatorType::comparison> {

  public:
    template <OperatorType OT>
    ComparisonSqlOperatorHelper(SqlOperatorBase<OT>&& sob, bool addNotInFront)
        : m_RequiredClassTypes(sob.requiredClassTypes()),
          m_Command((addNotInFront ? std::string("NOT ") : "") +
                    std::move(sob).to_string()) {}

    template <OperatorType OT_LHS, OperatorType OT_RHS>
    ComparisonSqlOperatorHelper(SqlOperatorBase<OT_LHS>&& sob0,
                                SqlOperatorBase<OT_RHS>&& sob1,
                                std::string_view operatorString)
        : m_RequiredClassTypes(CreateRequiredClassTypeVector(sob0, sob1)),
          m_Command(CreateCommandString(std::move(sob0), std::move(sob1),
                                        operatorString)) {}

    const std::vector<RequiredClassType>&
        requiredClassTypes() const noexcept override {
        return m_RequiredClassTypes;
    }

    std::string&& to_string() && noexcept override {
        return std::move(m_Command);
    }

  private:
    template <OperatorType OT_LHS, OperatorType OT_RHS>
    static std::vector<detail::RequiredClassType>
        CreateRequiredClassTypeVector(const SqlOperatorBase<OT_LHS>& sob0,
                                      const SqlOperatorBase<OT_RHS>& sob1) {
        auto result = sob0.requiredClassTypes();
        const auto& vec1 = sob1.requiredClassTypes();
        result.insert(result.end(), vec1.cbegin(), vec1.cend());
        return result;
    }
    template <OperatorType OT_LHS, OperatorType OT_RHS>
    static std::string CreateCommandString(SqlOperatorBase<OT_LHS>&& sob0,
                                           SqlOperatorBase<OT_RHS>&& sob1,
                                           std::string_view operatorString) {
        std::string&& operatorWithSpaces =
            ' ' + std::string(operatorString) + ' ';

        return std::move(sob0).to_string() + std::move(operatorWithSpaces) +
               std::move(sob1).to_string();
    }

  private:
    // order matters!
    const std::vector<detail::RequiredClassType> m_RequiredClassTypes;
    std::string m_Command;
};

template <OperatorType OT> static consteval void CheckOrAndAllowed() {
    static_assert(OT == OperatorType::comparison || OT == OperatorType::logical,
                  "Only logical or comparison types may use ! or || or &&");
}

template <OperatorType OT_LHS, OperatorType OT_RHS>
auto operator&&(SqlOperatorBase<OT_LHS>&& sob0,
                SqlOperatorBase<OT_RHS>&& sob1) {
    CheckOrAndAllowed<OT_LHS>();
    CheckOrAndAllowed<OT_RHS>();
    return ComparisonSqlOperatorHelper(std::move(sob0), std::move(sob1), "AND");
}
template <OperatorType OT_LHS, OperatorType OT_RHS>
auto operator||(SqlOperatorBase<OT_LHS>&& sob0,
                SqlOperatorBase<OT_RHS>&& sob1) {
    CheckOrAndAllowed<OT_LHS>();
    CheckOrAndAllowed<OT_RHS>();
    return ComparisonSqlOperatorHelper(std::move(sob0), std::move(sob1), "OR");
}
template <OperatorType OT> auto operator!(SqlOperatorBase<OT>&& sob) {
    return ComparisonSqlOperatorHelper(std::move(sob), true);
}

} // namespace sql_cpp::detail
