#pragma once

#include "sql_cpp/detail/create_table.hpp"
#include "sql_cpp/detail/delete_from.hpp"
#include "sql_cpp/detail/drop_table.hpp"
#include "sql_cpp/detail/insert.hpp"
#include "sql_cpp/detail/select_from.hpp"

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/table.hpp"
#include "sql_cpp/detail/util.hpp"

#include "sql_cpp/detail/operatorbase.hpp"

#include "sql_cpp/detail/where_clause.hpp"
#include "sql_cpp/order_by.hpp"

#include <sqlite3.h>
#include <vector>

namespace sql_cpp {

struct SqlCpp {

    using WHERE_CLAUSE = detail::WhereClause;
    template <auto... MemberPtr> using ORDER_BY = detail::OrderBy<MemberPtr...>;

    template <typename TableStruct> void drop_table();
    template <typename TableStruct> void create_table();

    template <typename TableStruct> void insert(const TableStruct& tableStruct);
    template <typename TableStruct_FwdIter,
              typename TableStruct_FwdIter_Sentinal>
    void insert(TableStruct_FwdIter begin, TableStruct_FwdIter_Sentinal end);

    template <typename TableStruct> void delete_from(WHERE_CLAUSE&& where);

    template <typename TableStruct,
              template <typename> typename ContainerType = std::vector>
    ContainerType<TableStruct> select_from();

    template <typename TableStruct,
              template <typename> typename ContainerType = std::vector>
    ContainerType<TableStruct> select_from(WHERE_CLAUSE&& where);

    template <typename TableStruct,
              template <typename> typename ContainerType = std::vector,
              auto... MemberPtr>
    ContainerType<TableStruct> select_from(ORDER_BY<MemberPtr...>&& orderby);

    template <typename TableStruct,
              template <typename> typename ContainerType = std::vector,
              auto... MemberPtr>
    ContainerType<TableStruct> select_from(WHERE_CLAUSE&& where,
                                           ORDER_BY<MemberPtr...>&& orderby);

  private:
};

template <typename TableStruct> void SqlCpp::drop_table() {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    constexpr auto sqlString = detail::Generate_DropTableString<TableType>();
    std::cout << sqlString.to_string_view() << std::endl;
}
template <typename TableStruct> void SqlCpp::create_table() {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    constexpr auto sqlString = detail::Generate_CreateTableString<TableType>();
    std::cout << sqlString.to_string_view() << std::endl;
}
template <typename TableStruct>
void SqlCpp::insert(const TableStruct& tableStruct) {
    return insert(&tableStruct, std::next(&tableStruct));
}

template <typename TableStruct_FwdIter, typename TableStruct_FwdIter_Sentinal>
void SqlCpp::insert(TableStruct_FwdIter begin,
                    TableStruct_FwdIter_Sentinal end) {
    using TableStruct = std::decay_t<decltype(*begin)>;
    using TableType = detail::TableTypeSelector_t<TableStruct>;

    const auto sqlString =
        detail::Generate_InsertTableString<TableType>(begin, end);
    std::cout << sqlString << std::endl;
}

template <typename TableStruct> void SqlCpp::delete_from(WHERE_CLAUSE&& where) {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    const auto sqlString =
        detail::Generate_DeleteFromString<TableType>(std::move(where));

    std::cout << sqlString << std::endl;
}

template <typename TableStruct, template <typename> typename ContainerType>
ContainerType<TableStruct> SqlCpp::select_from() {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    const auto sqlString = detail::Generate_SelectAllFromString<TableType>();
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStruct, template <typename> typename ContainerType>
ContainerType<TableStruct> SqlCpp::select_from(WHERE_CLAUSE&& where) {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    const auto sqlString =
        detail::Generate_SelectAllFromString<TableType>(std::move(where));
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStruct, template <typename> typename ContainerType,
          auto... MemberPtr>
ContainerType<TableStruct>
    SqlCpp::select_from(ORDER_BY<MemberPtr...>&& orderby) {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    const auto sqlString =
        detail::Generate_SelectAllFromString<TableType>(std::move(orderby));
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStruct, template <typename> typename ContainerType,
          auto... MemberPtr>
ContainerType<TableStruct>
    SqlCpp::select_from(WHERE_CLAUSE&& where,
                        ORDER_BY<MemberPtr...>&& orderby) {
    using TableType = detail::TableTypeSelector_t<TableStruct>;
    const auto sqlString = detail::Generate_SelectAllFromString<TableType>(
        std::move(where), std::move(orderby));
    std::cout << sqlString << std::endl;
    return {};
}

} // namespace sql_cpp
