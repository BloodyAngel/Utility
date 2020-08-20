
#include "sql_cpp/detail/static_string.hpp"
#include "sql_cpp/sql_cpp.hpp"
#include "sql_cpp/table.hpp"

struct Person {
    uint8_t age;
    std::string name;
};

int main() {
    sql_cpp::Table<Person, &Person::age, &Person::name> tb0;
    std::cout << tb0.GetTableName().to_string() << std::endl;
    std::cout << tb0.GetColumenNameAndType(0).to_string() << std::endl;
    std::cout << tb0.GetColumenNameAndType(1).to_string() << std::endl;
//    std::cout << tb0.GetColumenName(0).to_string() << '\t' << tb0.GetColumenTypeString(0).to_string() << std::endl;
//    std::cout << tb0.GetColumenName(1).to_string() << '\t' << tb0.GetColumenTypeString(1).to_string() << '\n' << std::endl;

    sql_cpp::Table<Person> tb1;
    std::cout << tb1.GetTableName().to_string_view() << std::endl;
    std::cout << tb1.GetColumenNameAndType(0).to_string() << std::endl;
    std::cout << tb1.GetColumenNameAndType(1).to_string() << std::endl;
//    std::cout << tb1.GetColumenName(0).to_string() << '\t' << tb1.GetColumenTypeString(0).to_string() << std::endl;
//    std::cout << tb1.GetColumenName(1).to_string() << '\t' << tb1.GetColumenTypeString(1).to_string() << '\n' << std::endl;

    return 0;
}
