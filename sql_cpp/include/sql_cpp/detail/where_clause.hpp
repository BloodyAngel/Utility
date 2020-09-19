#pragma once

#include "sql_cpp/detail/operatorbase.hpp"

namespace sql_cpp::detail {

class WhereClause final {
  public:
    WhereClause(SqlOperatorBase<OperatorType::comparison>&& comparison)
        : m_RequiredClassTypes(comparison.requiredClassTypes()),
          m_CompleteClause("WHERE " + std::move(comparison).to_string()) {}

    const std::vector<RequiredClassType>& requiredClassTypes() const noexcept {
        return m_RequiredClassTypes;
    }

    std::string&& to_string() && noexcept {
        return std::move(m_CompleteClause);
    }

  private:
    const std::vector<RequiredClassType> m_RequiredClassTypes;
    std::string m_CompleteClause;
};

} // namespace sql_cpp::detail
