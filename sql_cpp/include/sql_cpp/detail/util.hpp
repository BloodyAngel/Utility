#pragma once

#include "sql_cpp/detail/static_string.hpp"

#include <string_view>
#include <type_traits>

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

template <typename T, template <typename> typename U> struct IsTemplateOf : std::false_type {};
template <typename T, template <typename> typename U> struct IsTemplateOf<U<T>, U> : std::true_type {};

template <typename T, template <typename> typename U> static constexpr bool IsTemplateOf_v = IsTemplateOf<T, U>::value;

template <auto MemberPtr> class MemberPointerTraits {
    static_assert(std::is_member_object_pointer_v<decltype(MemberPtr)>, "No Member Ptr");

    // only declaration, no implementation
    template <typename ClassType, typename MemberType> static ClassType GetClassType(MemberType ClassType::*);
    template <typename ClassType, typename MemberType> static MemberType GetValueType(MemberType ClassType::*);

  public:
    using class_type = decltype(GetClassType(MemberPtr));
    using value_type = decltype(GetValueType(MemberPtr));
};

} // namespace sql_cpp::detail
