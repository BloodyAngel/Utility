#include "threadpool.hpp"
#include <iostream>

static void exampleWithoutPriority() {
    using namespace std::chrono_literals;
    ThreadPool pool;
    std::this_thread::sleep_for(1s);

    // example with return statements
    auto future0 = pool.dispatchWork([] {
        std::cout << "hello world 0" << std::endl;
        return 0;
    });
    auto future1 = pool.dispatchWork(
        [](int val) {
            std::cout << "hello world " << val << std::endl;
            return val;
        },
        1);

    // example without return statement
    // no future is returned!
    auto functionNoReturn = [] {
        std::cout << "hello world no return" << std::endl;
    };
    pool.dispatchWork(functionNoReturn);
    static_assert(
        std::is_same_v<decltype(pool.dispatchWork(functionNoReturn)), void>,
        "Non Void return");

    future0.wait();
    future1.wait();
    std::cout << "Return values: " << future0.get() << ", " << future1.get()
              << std::endl;
}

static void exampleWithPriority() {
    using namespace std::chrono_literals;
    ThreadPool pool(1);

    // dispatch some work that take a bit longer,
    // so we can add low/high priority tasks
    pool.dispatchWork([] { std::this_thread::sleep_for(100ms); });

    auto future0 = pool.dispatchWork(0, [] {
        std::cout << "Low Priority job\n" << std::flush;
        return 0;
    });
    auto future1 = pool.dispatchWork(1, [] {
        std::cout << "High Priority job\n" << std::flush;
        return 0;
    });

    // futures return by the threadpool are not blocking on there destructors,
    // therefore we have to explicitly wait for those
    future0.wait();
    future1.wait();

    // note: the threadpools destrutor ignores all currently dispatched tasks
    // which aren't running, however it will finish the currently running tasks
}

int main() {
    exampleWithoutPriority();
    exampleWithPriority();
    return 0;
}
