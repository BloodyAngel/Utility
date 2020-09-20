#pragma once

#include <cinttypes>
#include <string>
#include <variant>
#include <vector>
#include <chrono>

namespace sql_cpp::detail {

struct DbBind {
    unsigned index;

    /// TODO: add date
    std::variant<int64_t, uint64_t, bool, double, std::string, std::u8string,
                 std::u16string, std::u32string, std::vector<std::byte>>
        data;
};

} // namespace sql_cpp::detail
