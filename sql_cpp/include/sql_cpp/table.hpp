#pragma once

#include <boost/pfr.hpp>

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/util.hpp"

#include <cassert>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <variant>

namespace sql_cpp {

namespace detail {

template <typename TableType> struct TableBase {
    using value_type = TableType;
    static consteval auto GetTableName() { return detail::GetClassName<TableType>(); }

    template <typename ParameterType> static consteval detail::StaticString<> GetSqlTypeString() {
        using namespace std::string_view_literals;
        using TypeBase = std::decay_t<ParameterType>;
        if constexpr (std::is_integral_v<TypeBase>)
            return "INTEGER"sv;
        else if constexpr (std::is_floating_point_v<TypeBase>)
            return "REAL"sv;
        else if constexpr (detail::IsTemplateBase_v<TypeBase, std::basic_string>)
            return "TEXT"sv;
        else
            return "BLOB"sv;
    }

    /**
     * not yet supported in gcc 10
     * virtual consteval auto getColumenName() = 0;
     * virtual consteval auto getColumenCount() = 0;
     */
};

} // namespace detail

template <typename TableType, auto... MemberPointer> struct Table : public detail::TableBase<TableType> {

    static consteval detail::StaticString<> GetColumenName(unsigned index) {
        using namespace std::string_view_literals;
        if (index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return GetColumenName_Helper<0, MemberPointer...>(index);
    }

    static consteval unsigned GetColumnCount() { return sizeof...(MemberPointer); }

  private:
    template <unsigned CurrentIndex, auto FrontMemberPtr, auto... MemberPointerRest>
    static consteval detail::StaticString<> GetColumenName_Helper(unsigned desiredIndex) {
        if constexpr (sizeof...(MemberPointerRest) == 0) {
            if (CurrentIndex != desiredIndex)
                throw std::out_of_range("index out of range");

            return detail::GetMemberName<FrontMemberPtr>();
        } else {
            if (desiredIndex == CurrentIndex)
                return detail::GetMemberName<FrontMemberPtr>();

            return GetColumenName_Helper<CurrentIndex + 1, MemberPointerRest...>(desiredIndex);
        }
    }
};

template <typename TableType> struct Table<TableType> : public detail::TableBase<TableType> {

    static consteval detail::StaticString<> GetColumenName(unsigned index) {
        using namespace std::string_view_literals;
        if (index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return detail::StaticString<>("column_"sv) + UnsignedToStringView(index).to_string_view();
    }

    static consteval unsigned GetColumnCount() { return boost::pfr::tuple_size_v<TableType>; }

  private:
    static consteval detail::StaticString<> UnsignedToStringView(unsigned value) {
        if (value > 999)
            throw std::out_of_range("currently only supports up to 999");

        char tmpBuffer[4] = "000";
        std::for_each(std::rbegin(tmpBuffer) + 1, std::rend(tmpBuffer), [&value](auto& e) {
            e = '0' + (value % 10);
            value /= 10;
        });
        return std::string_view(std::cbegin(tmpBuffer), std::cend(tmpBuffer));
    }
};

} // namespace sql_cpp
