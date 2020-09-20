#include "sql_cpp/column_list.hpp"
#include "sql_cpp/operators.hpp"
#include "sql_cpp/sql_cpp.hpp"

#include "sql_cpp/order_by.hpp"

struct Person {
    uint8_t age = 0;
    std::string name = "hans";
};

struct Person2 {
    uint8_t age = 1;
    std::string name = "peter";
    using sql_cpp_column_list_type =
        sql_cpp::ColumnList<&Person2::age, &Person2::name>;
};

int main() {

    sql_cpp::SqlCpp sql;
    sql.create_table<Person>();
    sql.select_from<Person>();
    sql.select_from<sql_cpp::ColumnList<&Person::age>>();
    sql.select_from<&Person2::age>();
    sql.select_from<Person>(sql_cpp::order_by<&Person::age>());
    sql.select_from<Person>(sql_cpp::operators(&Person::age) < 1);

    static constexpr auto PersonAge = sql_cpp::operators(&Person::age);
    sql.select_from<&Person::age>(
        PersonAge < 100 && PersonAge >= 17,
        sql_cpp::order_by<&Person::age>(sql_cpp::Order::DESC));

    sql.insert(Person());
    sql.delete_from<Person>(sql_cpp::operators(&Person::age) < 100);

    std::array<Person, 4> people;
    std::fill(people.begin(), people.end(), Person());
    sql.insert(people.cbegin(), people.cend());
    sql.drop_table<Person>();

    std::cout << std::endl;

    sql.create_table<Person2>();
    sql.insert(Person2());
    std::array<Person2, 4> people2;
    std::fill(people2.begin(), people2.end(), Person2());
    sql.insert(people2.cbegin(), people2.cend());
    sql.drop_table<Person2>();

    std::cout << std::endl;

    return 0;
}
