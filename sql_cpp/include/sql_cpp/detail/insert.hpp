#pragma once

#include "sql_cpp/table.hpp"

#include <utility>

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_InsertTableString_CreateAllColumnNames() {
    using namespace std::string_view_literals;
    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::GetColumenName(CurrentIndex);
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount())
        return TableType::GetColumenName(CurrentIndex) + ", "sv +
               Generate_InsertTableString_CreateAllColumnNames<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}
template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_InsertTableString_CreateAllColumnValues() {
    using namespace std::string_view_literals;
    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::GetColumenName(CurrentIndex);
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount())
        return TableType::GetColumenName(CurrentIndex) + ", "sv +
               Generate_InsertTableString_CreateAllColumnNames<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}
template <typename TableType> static consteval auto Generate_InsertTableString(TableType&& tableType) {
    using namespace std::string_view_literals;
    constexpr auto baseString = "insert into "sv + TableType::GetTableName() + "("sv +
                                Generate_InsertTableString_CreateAllColumnNames<TableType>() + ") "sv;

    constexpr auto valueString = "("sv + Generate_InsertTableString_CreateAllColumnValues(std::forward(tableType)) + ")"sv;

    return baseString + valueString.to_string_view() + ";"sv;
}

} // namespace sql_cpp::detail
