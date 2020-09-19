#pragma once

#include "sql_cpp/detail/table.hpp"

#include <numeric>
#include <utility>

namespace sql_cpp::detail {

template <typename TableType, unsigned CurrentIndex = 0>
static consteval auto Generate_InsertTableString_CreateAllColumnNames() {

    using namespace std::string_view_literals;
    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::template GetColumenName<CurrentIndex>();
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount())
        return TableType::template GetColumenName<CurrentIndex>() + ", "sv +
               Generate_InsertTableString_CreateAllColumnNames<TableType, CurrentIndex + 1>().to_string_view();
    else
        throw std::out_of_range("index out of range");
}
template <typename TableType, unsigned CurrentIndex = 0>
static constexpr auto Generate_InsertTableString_CreateAllColumnValues(const typename TableType::value_type& tableType) {

    if constexpr (CurrentIndex + 1 == TableType::GetColumnCount())
        return TableType::template GetSqlValueStringFromIndex<CurrentIndex>(tableType);
    else if constexpr (CurrentIndex + 1 < TableType::GetColumnCount()) {
        const auto currentValueString = TableType::template GetSqlValueStringFromIndex<CurrentIndex>(tableType);
        const auto followingValueStrings =
            Generate_InsertTableString_CreateAllColumnValues<TableType, CurrentIndex + 1>(tableType);

        return currentValueString.empty() ? followingValueStrings : currentValueString + ", " + followingValueStrings;
    } else
        throw std::out_of_range("index out of range");
}
template <typename TableType, typename FwdIter, typename Sentinal>
static constexpr auto Generate_InsertTableString(FwdIter begin, Sentinal end) {

    using namespace std::string_view_literals;
    constexpr auto baseString = StaticString<>() + "INSERT INTO "sv + TableType::GetTableName().to_string_view() + "("sv +
                                Generate_InsertTableString_CreateAllColumnNames<TableType>().to_string_view() + ") VALUES"sv;

    const auto valueStrings = std::accumulate(begin, end, std::string(), [](const auto& current, const auto& tableType) {
        return current + ", (" + Generate_InsertTableString_CreateAllColumnValues<TableType>(tableType) + ')';
    });
    auto&& subStr = valueStrings.substr(1); // first ',' is not allowed
    return baseString.to_string() + std::move(subStr) + ';';
}

} // namespace sql_cpp::detail
