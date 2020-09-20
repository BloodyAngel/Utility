#pragma once

#include "sql_cpp/detail/operatorbase.hpp"
#include "sql_cpp/order_by.hpp"

namespace sql_cpp::detail {

template <typename TableStructOrColumnList, auto...>
struct GenerateSelectStringHelper {
    static std::string GetSelectBegin() {
        using TableType = TableTypeSelector_t<TableStructOrColumnList>;
        return "SELECT * FROM " + TableType::GetTableName().to_string();
    }
};

template <auto FirstMemberPtr, auto... MemberPtr>
struct GenerateSelectStringHelper<
    sql_cpp::ColumnList<FirstMemberPtr, MemberPtr...>> {

    using ClassType = typename MemberPointerTraits<FirstMemberPtr>::class_type;
    using TableType = TableTypeSelector_t<ClassType>;

    template <auto FrontPtr, auto... Ptrs>
    static std::string GetSelectBegin_AllMemberPtrStringHelper() {
        using namespace std::string_view_literals;
        const auto frontName = TableType::GetTableName().to_string() + '.' +
                               TableType::GetColumenName(FrontPtr).to_string();

        if constexpr (sizeof...(Ptrs)) {
            return frontName + ',' +
                   GetSelectBegin_AllMemberPtrStringHelper<Ptrs...>()
                       .to_string();
        }
        return frontName;
    }

    static std::string GetSelectBegin() {
        return "SELECT " +
               GetSelectBegin_AllMemberPtrStringHelper<FirstMemberPtr,
                                                       MemberPtr...>() +
               " FROM " + TableType::GetTableName().to_string();
    }
};

template <typename TableStructOrColumnList>
static std::string Generate_SelectFromString() {
    const auto selectBegin =
        GenerateSelectStringHelper<TableStructOrColumnList>::GetSelectBegin();

    return selectBegin + ';';
}
template <typename TableStructOrColumnList, typename OrderByOrWhere_Clause>
static std::string Generate_SelectFromString(OrderByOrWhere_Clause&& clause) {
    const auto selectBegin =
        GenerateSelectStringHelper<TableStructOrColumnList>::GetSelectBegin();

    return selectBegin + ' ' +
           std::forward<OrderByOrWhere_Clause>(clause).to_string() + ';';
}
template <typename TableStructOrColumnList, typename OrderByOrWhere_Clause0,
          typename OrderByOrWhere_Clause1>
static std::string Generate_SelectFromString(OrderByOrWhere_Clause0&& clause0,
                                             OrderByOrWhere_Clause1&& clause1) {
    const auto selectBegin =
        GenerateSelectStringHelper<TableStructOrColumnList>::GetSelectBegin();

    return selectBegin + ' ' +
           std::forward<OrderByOrWhere_Clause0>(clause0).to_string() + ' ' +
           std::forward<OrderByOrWhere_Clause1>(clause1).to_string() + ';';
}

} // namespace sql_cpp::detail
