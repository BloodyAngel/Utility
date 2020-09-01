#include "sql_cpp/column_list.hpp"
#include "sql_cpp/sql_cpp.hpp"

#include "sql_cpp/detail/create_table.hpp"
#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/detail/table.hpp"

struct Person {
    uint8_t age;
    std::string name;
};

struct Person2 {
    uint8_t age;
    std::string name;
};

int main() {

    using test = sql_cpp::ColumnList<&Person::age, &Person::name>;
    test::CheckConsistency();

    sql_cpp::SqlCpp sql;
    sql.createTable<Person>();
    sql.insert(Person());

    std::array<Person, 4> people;
    std::fill(people.begin(), people.end(), Person());
    sql.insert(people.cbegin(), people.cend());
    sql.dropTable<Person>();
    std::cout << std::endl;

    return 0;
}
