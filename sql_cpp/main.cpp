
#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/sql_cpp.hpp"
#include "sql_cpp/table.hpp"

#include "sql_cpp/detail/create_table.hpp"

struct Person {
    uint8_t age;
    std::string name;
};

int main() {
    sql_cpp::Table<Person, &Person::age, &Person::name> tb0;
    std::cout << tb0.GetTableName().to_string() << std::endl;
    std::cout << tb0.GetColumenNameAndType<0>().to_string() << std::endl;
    std::cout << tb0.GetColumenNameAndType<1>().to_string() << std::endl;

    sql_cpp::Table<Person> tb1;
    std::cout << tb1.GetTableName().to_string_view() << std::endl;
    std::cout << tb1.GetColumenNameAndType<0>().to_string() << std::endl;
    std::cout << tb1.GetColumenNameAndType<1>().to_string() << std::endl;

    std::cout << "\n" << std::endl;
    std::cout << sql_cpp::detail::Generate_CreateTableString<sql_cpp::Table<Person, &Person::name, &Person::age>>().to_string() << std::endl;
    std::cout << std::endl;

    Person p { .age = 123, .name = "peter" };
    sql_cpp::Table<Person> table_ref(&p);
    sql_cpp::Table<Person> table(p);
    table->age = 222;
    table->name = "hans";

    std::cout << int(p.age) << ' ' << p.name << std::endl;;
    std::cout << int(table->age) << ' ' << (*table).name << std::endl;;
    std::cout << int(table_ref->age) << ' ' << (*table_ref).name << std::endl;;
    std::cout << std::endl;

    sql_cpp::SqlCpp sql;
    sql.createTable<Person>();
    sql.insert(Person());
    sql.dropTable<Person>();
    std::cout << std::endl;

    return 0;
}
