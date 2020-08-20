
#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/sql_cpp.hpp"
#include "sql_cpp/table.hpp"

struct Person {
    uint8_t age;
    std::string name;
};

int main() {
    sql_cpp::Table<Person, &Person::age, &Person::name> tb;
    std::cout << tb.GetTableName().to_string() << std::endl;
    std::cout << tb.GetColumenName(0).to_string() << std::endl;
    std::cout << tb.GetColumenName(1).to_string() << '\n' << std::endl;

    sql_cpp::Table<Person> tb2;
    std::cout << tb2.GetTableName().to_string_view() << std::endl;
    std::cout << tb2.GetColumenName(0).to_string() << std::endl;
    std::cout << tb2.GetColumenName(1).to_string() << '\n' << std::endl;

    return 0;
}
