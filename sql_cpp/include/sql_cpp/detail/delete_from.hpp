#pragma once

#include "sql_cpp/detail/operatorbase.hpp"

namespace sql_cpp::detail {

template <typename TableType>
static auto Generate_DeleteFromString(
    SqlOperatorBase<OperatorType::comparison>&& where) {

    /// TODO: check only correct type is required
    return "DELETE FROM " + TableType::GetTableName().to_string() + " WHERE " +
           std::move(where).to_string() + ';';
}

} // namespace sql_cpp::detail
