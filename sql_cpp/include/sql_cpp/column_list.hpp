#pragma once

#include "sql_cpp/detail/util.hpp"
#include "sql_cpp/detail/table.hpp"

namespace sql_cpp {

template <auto FirstMemberPtr, auto... OtherMemberPtr> struct ColumnList {

    static_assert(std::is_member_pointer_v<decltype(FirstMemberPtr)>, "ColumnList contains non member pointer types");

    using class_type = detail::MemberPointerTraits<FirstMemberPtr>::class_type;
    using table_type = detail::Table<class_type, FirstMemberPtr, OtherMemberPtr...>;

    static consteval void CheckConsistency() { static_assert(CheckSameClassType<FirstMemberPtr, OtherMemberPtr...>(), ""); }

  private:
    template <auto First, auto... Args> static consteval bool CheckSameClassType() {
        using CurrentClassMemberPtrType = detail::MemberPointerTraits<First>::class_type;
        static_assert(std::is_member_pointer_v<decltype(First)>, "ColumnList contains non member pointer types");
        static_assert(std::is_same_v<class_type, CurrentClassMemberPtrType>,
                      "ColumnList contains pointer to different classes");

        if constexpr (sizeof...(Args))
            return CheckSameClassType<Args...>();
        else
            return true;
    }
};

} // namespace sql_cpp
