#include "sql_cpp/column_list.hpp"
#include <type_traits>

#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/sql_cpp.hpp"

#include "sql_cpp/detail/create_table.hpp"
#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/table.hpp"
#include "sql_cpp/operators.hpp"

struct Person {
    uint8_t age = 0;
    std::string name = "hans";
};

struct Person2 {
    uint8_t age = 1;
    std::string name = "peter";
    using sql_cpp_column_list_type = sql_cpp::ColumnList<&Person2::age, &Person2::name>;
};

int main() {
    using test = sql_cpp::ColumnList<&Person::age, &Person::name>;
    test::CheckConsistency();

    std::cout << sql_cpp::between(&Person::age, 1, 2).to_string() << std::endl;
    std::cout << sql_cpp::in(&Person::age, 1, 2).to_string() << std::endl;

    sql_cpp::SqlCpp sql;
    sql.createTable<Person>();
    sql.insert(Person());

    std::array<Person, 4> people;
    std::fill(people.begin(), people.end(), Person());
    sql.insert(people.cbegin(), people.cend());
    sql.dropTable<Person>();


    std::cout << std::endl;

    sql.createTable<Person2>();
    sql.insert(Person2());
    std::array<Person2, 4> people2;
    std::fill(people2.begin(), people2.end(), Person2());
    sql.insert(people2.cbegin(), people2.cend());
    sql.dropTable<Person2>();

    std::cout << std::endl;

    return 0;
}
