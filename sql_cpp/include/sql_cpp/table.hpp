#pragma once

#include <boost/pfr.hpp>

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/util.hpp"

#include <cassert>
#include <iostream>
#include <ranges>
#include <string>
#include <tuple>
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

    enum class SqlType { integer, real, text, blob };

    template <typename ParameterType> static consteval SqlType GetSqlType() {
        using TypeBase = std::decay_t<ParameterType>;
        if constexpr (std::is_integral_v<TypeBase> || std::is_enum_v<TypeBase>)
            return SqlType::integer;
        else if constexpr (std::is_floating_point_v<TypeBase>)
            return SqlType::real;
        else if constexpr (detail::IsTemplateBase_v<TypeBase, std::basic_string>)
            return SqlType::text;
        else
            return SqlType::blob;
    }

    template <typename ParameterType> static consteval detail::StaticString<> GetSqlTypeString() {
        using namespace std::string_view_literals;
        switch (GetSqlType<ParameterType>()) {
        case SqlType::integer:
            return "INTEGER"sv;
        case SqlType::real:
            return "REAL"sv;
        case SqlType::text:
            return "TEXT"sv;
        case SqlType::blob:
            return "BLOB"sv;
        }
        throw std::out_of_range("unkown SqlType");
    }

    template <typename ParameterType> static std::string GetSqlValueString(const ParameterType& parameterType) {
        using namespace std::string_view_literals;
        constexpr auto sqlType = GetSqlType<ParameterType>();
        if constexpr (sqlType == SqlType::integer || sqlType == SqlType::real)
            return std::to_string(parameterType);
        else if constexpr (sqlType == SqlType::text)
            return '\'' + EscapeSqlStringParameter(parameterType) + '\'';
        else if constexpr (sqlType == SqlType::blob) {
            /// TODO
            /// add auto serialize with boost::pfr
            throw std::runtime_error("blob's are not (yet) supported");
        }
        throw std::out_of_range("unkown SqlType");
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
    const TableType* operator->() const {
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

  private:
    static constexpr bool IsValidSqlStringCharacter(char c) {
        /// TODO: add correct function
        /// this function is very very restrictive at this point
        if (c >= '0' && c <= '9')
            return true;
        else if (c >= 'A' && c <= 'Z')
            return true;
        else if (c >= 'a' && c <= 'z')
            return true;
        else if (c == ' ')
            return true;
        return false;
    }
    static std::string EscapeSqlStringParameter(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        std::copy_if(str.cbegin(), str.cend(), std::back_inserter(result), IsValidSqlStringCharacter);
        return result;
    }
};

} // namespace detail

template <typename TableType, auto... MemberPointer> struct Table : public detail::TableBase<TableType> {
    Table(const TableType tableType) : detail::TableBase<TableType>(tableType) {}
    Table(TableType* const tableType = nullptr) : detail::TableBase<TableType>(tableType) {}

    template <unsigned Index> static consteval detail::StaticString<> GetColumenName() {
        return detail::GetMemberName<GetMemberPointer<Index>>();
    }

    static consteval unsigned GetColumnCount() { return sizeof...(MemberPointer); }

    template <unsigned Index> std::string getSqlValueString() const {
        TableType& self = *this;
        constexpr auto ptr = GetMemberPointer<Index>();
        return GetSqlValueString(self.*ptr);
    }

    template <unsigned Index> static consteval detail::StaticString<> GetColumenTypeString() {
        constexpr TableType* ptr = nullptr;
        using type = decltype(ptr->*GetMemberPointer<Index>());
        return detail::TableBase<TableType>::template GetSqlTypeString<type>();
    }

    template <unsigned Index> static consteval detail::StaticString<> GetColumenNameAndType() {
        using namespace std::string_view_literals;
        return GetColumenName<Index>() + " "sv + GetColumenTypeString<Index>().to_string_view();
    }

  private:
    template <unsigned Index> static consteval auto GetMemberPointer() {
        constexpr auto tuple = std::make_tuple(MemberPointer...);
        return std::get<Index>(tuple);
    }
};

template <typename TableType> struct Table<TableType> : public detail::TableBase<TableType> {
    Table(const TableType tableType) : detail::TableBase<TableType>(tableType) {}
    Table(TableType* const tableType = nullptr) : detail::TableBase<TableType>(tableType) {}

    template <unsigned Index> static consteval detail::StaticString<> GetColumenName() {
        using namespace std::string_view_literals;
        if (Index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return detail::StaticString<>("column_"sv) + UnsignedToStringView(Index).to_string_view();
    }

    static consteval unsigned GetColumnCount() { return boost::pfr::tuple_size_v<TableType>; }

    template <unsigned Index> std::string getSqlValueString() const {
        const TableType& self = *(this->operator->());
        const auto& value = boost::pfr::get<Index>(self);
        return detail::TableBase<TableType>::GetSqlValueString(value);
    }

    template <unsigned Index> static consteval detail::StaticString<> GetColumenTypeString() {
        using type = decltype(boost::pfr::get<Index>(TableType()));
        return detail::TableBase<TableType>::template GetSqlTypeString<type>();
    }

    template <unsigned Index> static consteval detail::StaticString<> GetColumenNameAndType() {
        using namespace std::string_view_literals;
        return GetColumenName<Index>() + " "sv + GetColumenTypeString<Index>().to_string_view();
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
};

} // namespace sql_cpp
