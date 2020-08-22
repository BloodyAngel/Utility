#pragma once

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/table.hpp"

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_CreateTableString_CreateAllColumnNamesAndTypes() {
    using namespace std::string_view_literals;
    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::template GetColumenNameAndType<CurrentIndex>();
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount())
        return TableType::template GetColumenNameAndType<CurrentIndex>() + ", "sv +
               Generate_CreateTableString_CreateAllColumnNamesAndTypes<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}
template <typename TableType> static consteval auto Generate_CreateTableString() {
    using namespace std::string_view_literals;

    constexpr auto CommandStart = StaticString<>("create table "sv) + (TableType::GetTableName() + "("sv).to_string_view();
    constexpr auto CommandParameter = Generate_CreateTableString_CreateAllColumnNamesAndTypes<TableType>();
    constexpr auto CommandEnd = ");"sv;

    return CommandStart + CommandParameter.to_string_view() + CommandEnd;
}

} // namespace sql_cpp::detail
