#pragma once

#include <sql_cpp/detail/static_string.hpp>
#include "sql_cpp/detail/table.hpp"

namespace sql_cpp::detail {

template <typename TableType> static consteval auto Generate_DropTableString() {

    using namespace std::string_view_literals;
    return StaticString<>("DROP TABLE "sv) + (TableType::GetTableName() + ";"sv).to_string_view();
}

} // namespace sql_cpp::detail
