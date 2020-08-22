#pragma once

#include "sql_cpp/table.hpp"

#include <utility>

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_InsertTableString_CreateAllColumnNames() {
    using namespace std::string_view_literals;
    using DecayedTableType = std::decay_t<TableType>;
    if constexpr (CurrentIndex + 1 == DecayedTableType::GetColumnCount())
        return DecayedTableType::template GetColumenName<CurrentIndex>();
    else if constexpr (CurrentIndex + 1 < DecayedTableType::GetColumnCount())
        return DecayedTableType::template GetColumenName<CurrentIndex>() + ", "sv +
               Generate_InsertTableString_CreateAllColumnNames<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}
template <typename TableType, unsigned CurrentIndex = 0>
static constexpr auto Generate_InsertTableString_CreateAllColumnValues(TableType&& tableType) {
    return std::string();
}
template <typename TableType> static constexpr auto Generate_InsertTableString(TableType&& tableType) {
    using namespace std::string_view_literals;
    using DecayedTableType = std::decay_t<TableType>;
    constexpr auto baseString = StaticString<>() + "insert into "sv + DecayedTableType::GetTableName().to_string_view() + "("sv +
                                Generate_InsertTableString_CreateAllColumnNames<DecayedTableType>().to_string_view() + ") "sv;

    auto valueString = '(' + Generate_InsertTableString_CreateAllColumnValues(std::forward<TableType>(tableType)) + ')';

    return baseString.to_string() + valueString + ';';
}

} // namespace sql_cpp::detail
