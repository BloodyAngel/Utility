#pragma once

#include <boost/pfr.hpp>
#include <boost/tti/has_type.hpp>

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/util.hpp"

#include <cassert>
#include <iostream>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>

BOOST_TTI_HAS_TYPE(sql_cpp_column_list_type)

namespace sql_cpp::detail {

template <typename TableStruct> struct TableBase {

    using value_type = TableStruct;

    static consteval auto GetTableName() { return GetClassName<TableStruct>(); }

    enum class SqlType { integer, real, text, blob };

    template <typename ParameterType> static consteval SqlType GetSqlType() {
        using TypeBase = std::decay_t<ParameterType>;
        if constexpr (std::is_integral_v<TypeBase> || std::is_enum_v<TypeBase>)
            return SqlType::integer;
        else if constexpr (std::is_floating_point_v<TypeBase>)
            return SqlType::real;
        else if constexpr (IsTemplateOf_v<TypeBase, std::basic_string>)
            return SqlType::text;
        else
            return SqlType::blob;
    }

    template <typename ParameterType>
    static consteval StaticString<> GetSqlTypeString() {
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

    template <typename ParameterType>
    static std::string GetSqlValueString(const ParameterType& parameterType) {
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
        std::copy_if(str.cbegin(), str.cend(), std::back_inserter(result),
                     IsValidSqlStringCharacter);
        return result;
    }
};

template <typename TableStruct, auto... MemberPointer>
struct Table final : public TableBase<TableStruct> {

    template <unsigned Index> static consteval StaticString<> GetColumenName() {
        return GetMemberName<GetMemberPointer<Index>()>();
    }

    template <typename T>
    static constexpr detail::StaticString<>
        GetColumenName(T TableStruct::*MemberObjectPointer) {
        if (!CheckMemberPointerExists(MemberObjectPointer)) {
            throw std::runtime_error(
                "MemberObjectPointer not part of ColumsList!");
        }
        return detail::GetMemberName<MemberObjectPointer>();
    }

    static consteval unsigned GetColumnCount() {
        return sizeof...(MemberPointer);
    }

    template <unsigned Index>
    static std::string
        GetSqlValueStringFromIndex(const TableStruct& tableStruct) {
        constexpr auto ptr = GetMemberPointer<Index>();
        return TableBase<TableStruct>::GetSqlValueString(tableStruct.*ptr);
    }

    template <unsigned Index>
    static consteval StaticString<> GetColumenTypeString() {
        constexpr TableStruct* ptr = nullptr;
        using type = decltype(ptr->*GetMemberPointer<Index>());
        return TableBase<TableStruct>::template GetSqlTypeString<type>();
    }

    template <unsigned Index>
    static consteval StaticString<> GetColumenNameAndType() {
        using namespace std::string_view_literals;
        return GetColumenName<Index>() + " "sv +
               GetColumenTypeString<Index>().to_string_view();
    }

  private:
    template <unsigned Index> static consteval auto GetMemberPointer() {
        constexpr auto tuple = std::make_tuple(MemberPointer...);
        return std::get<Index>(tuple);
    }
    template <typename T, unsigned CurrentIndex>
    static constexpr detail::StaticString<>
        GetColumnName_Helper(T TableStruct::*MemberObjectPointer) {
        if constexpr (CurrentIndex < sizeof...(MemberPointer)) {
            constexpr auto tuple = std::make_tuple(MemberPointer...);
            constexpr auto ptr = std::get<CurrentIndex>(tuple);
            if (ptr == MemberObjectPointer)
                return detail::GetMemberName<ptr>();

            return GetColumnName_Helper<CurrentIndex + 1>(MemberObjectPointer);
        } else
            throw std::runtime_error("Unable To find member pointer");
    }
};

template <typename TableStruct>
struct Table<TableStruct> final : public TableBase<TableStruct> {

    static constexpr auto CreateColumnNameFromIndex(unsigned index) {
        using namespace std::string_view_literals;
        const auto unsignedString = UnsignedToStringView(index);
        return detail::StaticString<>("column_"sv) +
               unsignedString.to_string_view();
    }

    template <typename T>
    static detail::StaticString<>
        GetColumenName(T TableStruct::*MemberObjectPointer) {
        static TableStruct tableStruct;
        unsigned index(-1);
        unsigned currentIndex = 0;
        const auto MemberPtr = &(tableStruct.*MemberObjectPointer);

        auto searchFunc = [MemberPtr, &index, &currentIndex](auto& element) {
            using ValueType = std::decay_t<decltype(element)>;
            if constexpr (std::is_same_v<ValueType, T>) {
                if (MemberPtr == &element)
                    index = currentIndex;
            }
            ++currentIndex;
        };
        boost::pfr::for_each_field(tableStruct, searchFunc);
        if (index >= GetColumnCount())
            throw std::runtime_error("MemberPointer out of range");

        return CreateColumnNameFromIndex(index);
    }

    template <unsigned Index> static consteval StaticString<> GetColumenName() {
        using namespace std::string_view_literals;
        if (Index >= GetColumnCount())
            throw std::out_of_range("index out of range");

        return StaticString<>("column_"sv) +
               UnsignedToStringView(Index).to_string_view();
    }

    static consteval unsigned GetColumnCount() {
        return boost::pfr::tuple_size_v<TableStruct>;
    }

    template <unsigned Index>
    static std::string
        GetSqlValueStringFromIndex(const TableStruct& tableStruct) {
        const auto& value = boost::pfr::get<Index>(tableStruct);
        return TableBase<TableStruct>::GetSqlValueString(value);
    }

    template <unsigned Index>
    static consteval StaticString<> GetColumenTypeString() {
        using type = decltype(boost::pfr::get<Index>(TableStruct()));
        return TableBase<TableStruct>::template GetSqlTypeString<type>();
    }

    template <unsigned Index>
    static consteval StaticString<> GetColumenNameAndType() {
        using namespace std::string_view_literals;
        return GetColumenName<Index>() + " "sv +
               GetColumenTypeString<Index>().to_string_view();
    }

  private:
    static constexpr StaticString<> UnsignedToStringView(unsigned value) {
        if (value > 999)
            throw std::out_of_range("currently only supports up to 999");

        char tmpBuffer[4] = "000";
        std::for_each(std::rbegin(tmpBuffer) + 1, std::rend(tmpBuffer),
                      [&value](auto& e) {
                          e = '0' + (value % 10);
                          value /= 10;
                      });
        return std::string_view(std::cbegin(tmpBuffer), std::cend(tmpBuffer));
    }
};

template <typename TalbeStruct,
          bool allColumns =
              has_type_sql_cpp_column_list_type<TalbeStruct>::value>
struct TableTypeSelector;

template <typename TableStruct> struct TableTypeSelector<TableStruct, false> {
    using type = Table<TableStruct>;
};
template <typename TableStruct> struct TableTypeSelector<TableStruct, true> {
    using type = typename TableStruct::sql_cpp_column_list_type::table_type;
};

template <typename TableStruct>
using TableTypeSelector_t = typename TableTypeSelector<TableStruct>::type;

} // namespace sql_cpp::detail
