#pragma once

#include <string>
#include <string_view>

namespace sql_cpp::detail {

/// TODO: constexpr once std::string is constexpr in the STL
class SqlClauseHelper {
  public:
    SqlClauseHelper(std::string_view clause, std::string_view parameter = "")
        : m_Clause(clause), m_Parameters(parameter) {}

    std::string_view getClause() const { return m_Clause; }

    std::string_view getParameter() const { return m_Parameters; }
    void setParameter(std::string_view parameter) { m_Parameters = parameter; }

    void appendParameter(std::string_view parameterToAppend) {
        m_Parameters.append(parameterToAppend);
    }

  private:
    std::string m_Clause;
    std::string m_Parameters;
};

class WhereClause {

  private:
    SqlClauseHelper m_Clause;
};

} // namespace sql_cpp::detail
