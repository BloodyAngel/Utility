#pragma once

#include "sql_cpp/detail/create_table.hpp"
#include "sql_cpp/detail/drop_table.hpp"
#include "sql_cpp/detail/insert.hpp"

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/util.hpp"
#include "sql_cpp/table.hpp"

#include <sqlite3.h>
#include <vector>

namespace sql_cpp {

struct SqlCpp {

    template <typename TableType> void dropTable();
    template <typename TableType> void createTable();
    template <typename TableType> void insert(const TableType& tableType);
    template <typename TableType_FwdIter> void insert(TableType_FwdIter begin, TableType_FwdIter end);

  private:
};

template <typename TableType> void SqlCpp::dropTable() {
    if constexpr (detail::IsTemplateBase_v<TableType, Table>) {
        constexpr auto sqlString = detail::Generate_DropTableString<TableType>();
        std::cout << "Drop table string: " << sqlString.to_string_view() << std::endl;
    } else
        return createTable<Table<TableType>>();
}
template <typename TableType> void SqlCpp::createTable() {
    if constexpr (detail::IsTemplateBase_v<TableType, Table>) {
        constexpr auto sqlString = detail::Generate_CreateTableString<TableType>();
        std::cout << "Create table string: " << sqlString.to_string_view() << std::endl;
    } else
        return createTable<Table<TableType>>();
}
template <typename TableType> void SqlCpp::insert(const TableType& tableType) {
    if constexpr (detail::IsTemplateBase_v<TableType, Table>) {
        constexpr auto sqlString = detail::Generate_InsertTableString(tableType);
        std::cout << "Insert string: " << sqlString.to_string_view() << std::endl;
    } else
        return insert(Table<TableType>(&tableType));
}

template <typename TableType_FwdIter> void SqlCpp::insert(TableType_FwdIter begin, TableType_FwdIter end) {
    /// simplified
    /// performance can be fastly improved!!
    std::for_each(begin, end, [this](const auto& tableType) { insert(tableType); });
}

} // namespace sql_cpp
