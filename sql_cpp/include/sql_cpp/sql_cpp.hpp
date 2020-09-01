#pragma once

#include "sql_cpp/detail/create_table.hpp"
#include "sql_cpp/detail/drop_table.hpp"
#include "sql_cpp/detail/insert.hpp"

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/table.hpp"
#include "sql_cpp/detail/util.hpp"

#include <sqlite3.h>
#include <vector>

namespace sql_cpp {

struct SqlCpp {

    template <typename TableStruct> void dropTable();
    template <typename TableStruct> void createTable();

    template <typename TableStruct> void insert(const TableStruct& tableStruct);
    template <typename TableStruct_FwdIter, typename TableStruct_FwdIter_Sentinal>
    void insert(TableStruct_FwdIter begin, TableStruct_FwdIter_Sentinal end);

  private:
};

template <typename TableStruct> void SqlCpp::dropTable() {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    constexpr auto sqlString = detail::Generate_DropTableString<TableType>();
    std::cout << sqlString.to_string_view() << std::endl;
}
template <typename TableStruct> void SqlCpp::createTable() {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    constexpr auto sqlString = detail::Generate_CreateTableString<TableType>();
    std::cout << sqlString.to_string_view() << std::endl;
}
template <typename TableStruct> void SqlCpp::insert(const TableStruct& tableStruct) {
    return insert(&tableStruct, std::next(&tableStruct));
}

template <typename TableStruct_FwdIter, typename TableStruct_FwdIter_Sentinal>
void SqlCpp::insert(TableStruct_FwdIter begin, TableStruct_FwdIter_Sentinal end) {
    using TableStruct = std::decay_t<decltype(*begin)>;
    using TableType = detail::TableTypeSelector_t<TableStruct>;

    const auto sqlString = detail::Generate_InsertTableString<TableType>(begin, end);
    std::cout << sqlString << std::endl;
}

} // namespace sql_cpp
