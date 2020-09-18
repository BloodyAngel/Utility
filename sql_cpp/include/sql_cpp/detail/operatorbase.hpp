#pragma once

#include <stdexcept>
#include <string>

#include "sql_cpp/detail/util.hpp"
#include "sql_cpp/detail/table.hpp"

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
        if constexpr (std::is_member_object_pointer_v<T>) {
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

} // namespace sql_cpp::detail
