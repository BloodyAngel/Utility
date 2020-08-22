#pragma once

#include "sql_cpp/table.hpp"

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_InsertTableString_CreateAllColumnNames(const TableType& tableType) {
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
template <typename TableType> static consteval auto Generate_InsertTableString(const TableType& tableType) {
    using namespace std::string_view_literals;
    constexpr auto baseString = "insert into "sv + TableType::GetTableName() + "("sv +
                                Generate_InsertTableString_CreateAllColumnNames<TableType>() + ") "sv;
    return baseString;
}

} // namespace sql_cpp::detail
