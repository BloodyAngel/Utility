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

    template <typename TableStructOrColumnList,
              template <typename, typename>
              typename ContainerType = std::vector,
              typename Allocator = std::allocator<TableStructOrColumnList>>
    ContainerType<TableStructOrColumnList, Allocator> select_from();

    template <auto... MemberPtr> auto select_from() {
        return select_from<sql_cpp::ColumnList<MemberPtr...>>();
    }

    template <typename TableStructOrColumnList,
              template <typename, typename>
              typename ContainerType = std::vector,
              typename Allocator = std::allocator<TableStructOrColumnList>>
    ContainerType<TableStructOrColumnList, Allocator>
        select_from(WHERE_CLAUSE&& where);

    template <auto... MemberPtr> auto select_from(WHERE_CLAUSE&& where) {
        return select_from<sql_cpp::ColumnList<MemberPtr...>>(std::move(where));
    }

    template <typename TableStructOrColumnList,
              template <typename, typename>
              typename ContainerType = std::vector,
              typename Allocator = std::allocator<TableStructOrColumnList>,
              auto... MemberPtr>
    ContainerType<TableStructOrColumnList, Allocator>
        select_from(ORDER_BY<MemberPtr...>&& orderby);

    template <auto... MemberPtr, auto... MemberPtr2>
    auto select_from(ORDER_BY<MemberPtr2...>&& orderby) {
        return select_from<sql_cpp::ColumnList<MemberPtr...>>(
            std::move(orderby));
    }

    template <typename TableStructOrColumnList,
              template <typename, typename>
              typename ContainerType = std::vector,
              typename Alocator = std::allocator<TableStructOrColumnList>,
              auto... MemberPtr>
    ContainerType<TableStructOrColumnList, Alocator>
        select_from(WHERE_CLAUSE&& where, ORDER_BY<MemberPtr...>&& orderby);

    template <auto... MemberPtr, auto... MemberPtr2>
    auto select_from(WHERE_CLAUSE&& where, ORDER_BY<MemberPtr2...>&& orderby) {
        return select_from<sql_cpp::ColumnList<MemberPtr...>>(
            std::move(where), std::move(orderby));
    }

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

template <typename TableStructOrColumnList,
          template <typename, typename> typename ContainerType,
          typename Allocator>
ContainerType<TableStructOrColumnList, Allocator> SqlCpp::select_from() {
    const auto sqlString =
        detail::Generate_SelectFromString<TableStructOrColumnList>();
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStructOrColumnList,
          template <typename, typename> typename ContainerType,
          typename Allocator>
ContainerType<TableStructOrColumnList, Allocator>
    SqlCpp::select_from(WHERE_CLAUSE&& where) {
    const auto sqlString =
        detail::Generate_SelectFromString<TableStructOrColumnList>(
            std::move(where));
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStructOrColumnList,
          template <typename, typename> typename ContainerType,
          typename Allocator, auto... MemberPtr>
ContainerType<TableStructOrColumnList, Allocator>
    SqlCpp::select_from(ORDER_BY<MemberPtr...>&& orderby) {
    const auto sqlString =
        detail::Generate_SelectFromString<TableStructOrColumnList>(
            std::move(orderby));
    std::cout << sqlString << std::endl;
    return {};
}

template <typename TableStructOrColumnList,
          template <typename, typename> typename ContainerType,
          typename Allocator, auto... MemberPtr>
ContainerType<TableStructOrColumnList, Allocator>
    SqlCpp::select_from(WHERE_CLAUSE&& where,
                        ORDER_BY<MemberPtr...>&& orderby) {
    const auto sqlString =
        detail::Generate_SelectFromString<TableStructOrColumnList>(
            std::move(where), std::move(orderby));
    std::cout << sqlString << std::endl;
    return {};
}

} // namespace sql_cpp
