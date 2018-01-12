#include <iostream>
#include "threadpool.hpp"

int main(){
    ThreadPool<> pool;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::future<int> future0 = pool.dispatchWork([]{ std::cout << "hello world" << std::endl; return 0; });
    std::future<int> future1 = pool.dispatchWork([]{ std::cout << "hello world" << std::endl; return 0; });
    std::future<int> future2 = pool.dispatchWork([](int val){ std::cout << "hello world " << val << std::endl; return 0; }, 2);
    pool.dispatchWork([]{ std::cout << "yes" << std::endl; });
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << future0.get() << std::endl;
    std::cout << future1.get() << std::endl;
    std::cout << future2.get() << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
