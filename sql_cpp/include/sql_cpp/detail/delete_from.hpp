#pragma once

#include "sql_cpp/detail/where_clause.hpp"

namespace sql_cpp::detail {

template <typename TableType>
static auto Generate_DeleteFromString(WhereClause&& where) {

    /// TODO: check only correct type is required
    return "DELETE FROM " + TableType::GetTableName().to_string() + ' ' +
           std::move(where).to_string() + ';';
}

} // namespace sql_cpp::detail
