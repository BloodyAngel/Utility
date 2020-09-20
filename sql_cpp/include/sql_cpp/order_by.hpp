#pragma once

#include "sql_cpp/column_list.hpp"
#include "sql_cpp/detail/operatorbase.hpp"

namespace sql_cpp {

enum class Order { ASC, DESC };

namespace detail {

template <auto FirstMemberPtr, auto... OtherMemberPtr> class OrderBy {
    using CL = ColumnList<FirstMemberPtr, OtherMemberPtr...>;

  public:
    using class_type = typename CL::class_type;

    OrderBy(Order order) : m_Command(CreateCommand(order)), m_Order(order) {
        CL::CheckConsistency();
    }

    std::vector<RequiredClassType>&& requiredClassTypes() const noexcept {
        return {TypeToRequiredClassType<class_type>()};
    }

    std::string&& to_string() && noexcept { return std::move(m_Command); }

  private:
    template <auto FrontPtr, auto... OtherPtrs>
    static std::string CreateMemberListCommand() {
        const auto name = detail::GetMemberName<FrontPtr>().to_string();
        if constexpr (sizeof...(OtherPtrs))
            return name + ',' + CreateMemberListCommand<OtherPtrs...>();
        else
            return name;
    }

    static std::string CreateCommand(Order order) {
        const char* orderStr = order == Order::ASC ? " ASC" : " DESC";
        return "ORDER BY " +
               CreateMemberListCommand<FirstMemberPtr, OtherMemberPtr...>() +
               orderStr;
    }

    std::string m_Command;
    const Order m_Order;
};

} // namespace detail

template <auto FirstMemberPtr, auto... OtherMemberPtr>
auto order_by(Order order = Order::ASC) {
    return detail::OrderBy<FirstMemberPtr, OtherMemberPtr...>(order);
}

} // namespace sql_cpp
