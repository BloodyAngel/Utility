#include <iostream>
#include <algorithm>

#include "dynamicarray.hpp"


int main()
{
    angel::DynamicArray<int> ints = {7, 2, 3 ,-24};
    std::sort(std::begin(ints), std::end(ints));

    for (auto& e : ints)
        std::cout << e << std::endl;

    return EXIT_SUCCESS;
}
