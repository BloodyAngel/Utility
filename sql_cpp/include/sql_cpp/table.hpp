#pragma once

#include <boost/pfr.hpp>

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/util.hpp"

#include <cassert>
#include <iostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <variant>

namespace sql_cpp {

namespace detail {

/// 'overloaded' helper function used used from:
/// https://en.cppreference.com/w/cpp/utility/variant/visit

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename TableType> struct TableBase {
    using value_type = TableType;

    TableBase(const TableType tableType) : m_TableType(tableType) {}
    TableBase(TableType* const tableType = nullptr) : m_TableType(tableType) {}

    static consteval auto GetTableName() { return detail::GetClassName<TableType>(); }

    template <typename ParameterType> static consteval detail::StaticString<> GetSqlTypeString() {
        using namespace std::string_view_literals;
        using TypeBase = std::decay_t<ParameterType>;
        if constexpr (std::is_integral_v<TypeBase> || std::is_enum_v<TypeBase>)
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

    TableBase& operator=(TableType* const tableType) { m_TableType = tableType; }
    TableBase& operator=(const TableType& tableType) { m_TableType = tableType; }
    TableBase& operator=(TableType&& tableType) { m_TableType = std::move_if_noexcept(tableType); }

    TableType* operator->() {
        return std::visit(overloaded{
                              [](TableType& tableType) { return &tableType; },
                              [](TableType* tableType) { return tableType; },
                          },
                          m_TableType);
    }
    const TableType operator->() const {
        return std::visit(overloaded{
                              [](const TableType& tableType) { return &tableType; },
                              [](const TableType* tableType) { return tableType; },
                          },
                          m_TableType);
    }

    TableType& operator*() { return *(*this).operator->(); }
    const TableType& operator*() const { return *(*this).operator->(); }

  protected:
    std::variant<TableType, TableType*> m_TableType;
};

} // namespace detail

template <typename TableType, auto... MemberPointer> struct Table : public detail::TableBase<TableType> {
    Table(const TableType tableType) : detail::TableBase<TableType>(tableType) {}
    Table(TableType* const tableType = nullptr) : detail::TableBase<TableType>(tableType) {}

    static consteval detail::StaticString<> GetColumenName(unsigned index) {
        if (index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return GetColumenName_Helper<0, MemberPointer...>(index);
    }

    static consteval unsigned GetColumnCount() { return sizeof...(MemberPointer); }

    static consteval detail::StaticString<> GetColumenTypeString(unsigned index) {
        if (index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return GetColumenTypeString_Helper<0, MemberPointer...>(index);
    }

    static consteval detail::StaticString<> GetColumenNameAndType(unsigned index) {
        using namespace std::string_view_literals;
        return GetColumenName(index) + " "sv + GetColumenTypeString(index).to_string_view();
    }

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

    template <auto MemberPtr> static consteval detail::StaticString<> ConvertMemberPtr2TypeString() {
        constexpr TableType* ptr = nullptr;
        using type = decltype(ptr->*MemberPtr);
        return detail::TableBase<TableType>::template GetSqlTypeString<type>();
    }

    template <unsigned CurrentIndex, auto FrontMemberPtr, auto... MemberPointerRest>
    static consteval detail::StaticString<> GetColumenTypeString_Helper(unsigned desiredIndex) {
        if constexpr (sizeof...(MemberPointerRest) == 0) {
            if (desiredIndex != CurrentIndex)
                throw std::out_of_range("index out of range");

            return ConvertMemberPtr2TypeString<FrontMemberPtr>();
        } else {
            if (desiredIndex != CurrentIndex)
                return GetColumenTypeString_Helper<CurrentIndex + 1, MemberPointerRest...>(desiredIndex);

            return ConvertMemberPtr2TypeString<FrontMemberPtr>();
        }
    }
};

template <typename TableType> struct Table<TableType> : public detail::TableBase<TableType> {
    Table(const TableType tableType) : detail::TableBase<TableType>(tableType) {}
    Table(TableType* const tableType = nullptr) : detail::TableBase<TableType>(tableType) {}

    static consteval detail::StaticString<> GetColumenName(unsigned index) {
        using namespace std::string_view_literals;
        if (index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return detail::StaticString<>("column_"sv) + UnsignedToStringView(index).to_string_view();
    }

    static consteval unsigned GetColumnCount() { return boost::pfr::tuple_size_v<TableType>; }

    static consteval detail::StaticString<> GetColumenTypeString(unsigned index) {
        if (index >= boost::pfr::tuple_size_v<TableType>)
            throw std::out_of_range("index out of range");

        return GetColumenTypeString_Helper(index);
    }

    static consteval detail::StaticString<> GetColumenNameAndType(unsigned index) {
        using namespace std::string_view_literals;
        return GetColumenName(index) + " "sv + GetColumenTypeString(index).to_string_view();
    }

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

    template <unsigned CurrentIndex = 0> static consteval detail::StaticString<> GetColumenTypeString_Helper(unsigned index) {
        if constexpr (CurrentIndex >= GetColumnCount())
            throw std::out_of_range("index out of range");
        else {
            if (CurrentIndex != index)
                return GetColumenTypeString_Helper<CurrentIndex + 1>(index);

            using type = decltype(boost::pfr::get<CurrentIndex>(TableType()));
            return detail::TableBase<TableType>::template GetSqlTypeString<type>();
        }
    }
};

} // namespace sql_cpp
