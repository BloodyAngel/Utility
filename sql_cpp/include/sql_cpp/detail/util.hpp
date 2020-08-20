#pragma once

#include "sql_cpp/detail/static_string.hpp"

#include <string_view>

namespace sql_cpp::detail {

template <typename T> static consteval std::string_view GetClassNameStringView() {
    constexpr std::string_view funcName = __PRETTY_FUNCTION__;

    constexpr std::string_view EndString = ";";
    constexpr std::string_view StartString = " = ";

    constexpr auto beginIndex = funcName.find(StartString) + StartString.size();
    constexpr auto endIndex = funcName.find(EndString, beginIndex);

    static_assert(beginIndex < endIndex);
    static_assert(endIndex != std::string_view::npos);

    const auto cbegin = funcName.cbegin();
    return {cbegin + beginIndex, cbegin + endIndex};
}

template <typename T> static consteval StaticString<> GetClassName() {
    constexpr auto name = GetClassNameStringView<T>();
    static_assert(name.size() < StaticString<>::max_size());
    return name;
}

template <auto MemberPtr> static consteval StaticString<> GetMemberName() {
    constexpr std::string_view funcName = __PRETTY_FUNCTION__;

    constexpr std::string_view EndString = "]";
    constexpr std::string_view StartString = "::";

    constexpr auto beginIndex = funcName.rfind(StartString) + StartString.size();
    constexpr auto endIndex = funcName.rfind(EndString);

    static_assert(beginIndex < endIndex);
    static_assert(endIndex != std::string_view::npos);

    constexpr auto cbegin = funcName.cbegin();
    return std::string_view{cbegin + beginIndex, cbegin + endIndex};
}

} // namespace sql_cpp::detail
