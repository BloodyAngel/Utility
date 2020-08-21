#pragma once

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/table.hpp"

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_CreateTableString_CreateAllNamesAndTypes() {
    using namespace std::string_view_literals;
    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::GetColumenNameAndType(CurrentIndex);
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount())
        return TableType::GetColumenNameAndType(CurrentIndex) + ", "sv +
               Generate_CreateTableString_CreateAllNamesAndTypes<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}

template <typename TableType> static consteval auto Generate_CreateTableString() {
    using namespace std::string_view_literals;

    constexpr auto CommandStart = StaticString<>("create table "sv) + (TableType::GetTableName() + "("sv).to_string_view();
    constexpr auto CommandParameter = Generate_CreateTableString_CreateAllNamesAndTypes<TableType>();
    constexpr auto CommandEnd = ");"sv;

    return CommandStart + CommandParameter.to_string_view() + CommandEnd;
}

template <typename TableType> static consteval auto Generate_DropTableString() {
    using namespace std::string_view_literals;
    return StaticString<>("drop table "sv) + (TableType::GetTableName() + ";"sv);
}

} // namespace sql_cpp::detail
