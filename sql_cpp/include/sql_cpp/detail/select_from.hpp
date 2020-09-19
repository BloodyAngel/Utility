#pragma once

#include "sql_cpp/detail/operatorbase.hpp"
#include "sql_cpp/order_by.hpp"

namespace sql_cpp::detail {

static constexpr inline const char* SELECT_ALL_BASE = "SELECT * FROM ";

template <typename TableType>
static std::string Generate_SelectAllFromString() {
    return SELECT_ALL_BASE + TableType::GetTableName().to_string() + ';';
}
template <typename TableType, typename OrderByOrWhere_Clause>
static std::string
    Generate_SelectAllFromString(OrderByOrWhere_Clause&& clause) {
    return SELECT_ALL_BASE + TableType::GetTableName().to_string() + ' ' +
           std::forward<OrderByOrWhere_Clause>(clause).to_string() + ';';
}
template <typename TableType, typename OrderByOrWhere_Clause0,
          typename OrderByOrWhere_Clause1>
static std::string
    Generate_SelectAllFromString(OrderByOrWhere_Clause0&& clause0,
                                 OrderByOrWhere_Clause1&& clause1) {
    return SELECT_ALL_BASE + TableType::GetTableName().to_string() + ' ' +
           std::forward<OrderByOrWhere_Clause0>(clause0).to_string() + ' ' +
           std::forward<OrderByOrWhere_Clause1>(clause1).to_string() + ';';
}

} // namespace sql_cpp::detail
