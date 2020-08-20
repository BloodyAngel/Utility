#pragma once

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/table.hpp"

#include <sqlite3.h>
#include <vector>

namespace sql_cpp {

struct SqlCpp {

    template <typename TableType> void createTable();

  private:
};

template <typename TableType> void SqlCpp::createTable() {
}

} // namespace sql_cpp
