#pragma once
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <future>
#include <functional>

template<typename _PriorityType = int, typename _Compare = std::less<_PriorityType>>
struct ThreadPool final {
    ThreadPool(const size_t numThreads = std::thread::hardware_concurrency()) : m_Threads(numThreads){
        auto workerFunction = [this]{
            while (!this->m_Stop){
                std::unique_lock<std::mutex> lock(this->m_Mutex);
                this->m_ConditionVariable.wait(lock, [this]{ return this->m_Stop || !this->m_Queue.empty(); });
                if (this->m_Stop)
                    break;

                auto task = std::move(this->m_Queue.top().function);
                this->m_Queue.pop();
                lock.unlock();
                task();
            }
        };
        for (auto& e : this->m_Threads)
            e = std::thread(workerFunction);
    }
    ~ThreadPool(){
        this->m_Stop = true;
        this->m_ConditionVariable.notify_all();
        for (auto& e : this->m_Threads)
            e.join();
    }

    template<typename _PType, typename _Function, typename... _Args>
    auto dispatchWork(_PType&& priority, _Function&& function, _Args&&... args) -> typename std::enable_if<!std::is_same<decltype(function(args...)), void>::value, std::future<decltype(function(args...))>>::type {
        auto task = std::make_shared<std::packaged_task<decltype(function(args...))()>>(std::bind(std::forward<_Function>(function), std::forward<_Args>(args)...));
        auto future = task->get_future();
        this->_dispatch(std::forward<_PType>(priority), [task]{ (*task)(); });
        return future;
    }
    template<typename _PType, typename _Function, typename... _Args>
    auto dispatchWork(_PType&& priority, _Function&& function, _Args&&... args) -> typename std::enable_if<std::is_same<decltype(function(args...)), void>::value, void>::type{
        this->_dispatch(std::forward<_PType>(priority), std::bind(std::forward<_Function>(function), std::forward<_Args>(args)...));
    }

    template<typename _Function, typename... _Args>
    auto dispatchWork(_Function&& function, _Args&&... args) -> typename std::enable_if<!std::is_same<decltype(function(args...)), void>::value, std::future<decltype(function(args...))>>::type {
        return this->dispatchWork(_PriorityType(), std::forward<_Function>(function), std::forward<_Args>(args)...);
    }
    template<typename _Function, typename... _Args>
    auto dispatchWork(_Function&& function, _Args... args) -> typename std::enable_if<std::is_same<decltype(function(args...)), void>::value, void>::type{
        this->dispatchWork(_PriorityType(), std::forward<_Function>(function), std::forward<_Args>(args)...);
    }

private:
    struct _Work{
        _PriorityType priority;
        std::function<void()> function;

        template<typename _PType, typename _Function>
        _Work(_PType&& prio, _Function&& foo) : priority(std::forward<_PType>(prio)), function(std::forward<_Function>(foo)){}

        bool operator<(const _Work& rhs) const{
            static _Compare cmp;
            return cmp(this->priority, rhs.priority);
        }
    };
    template<typename _PType, typename _Function>
    void _dispatch(_PType&& priority, _Function&& function){
        std::unique_lock<std::mutex> lock(this->m_Mutex);
        this->m_Queue.emplace(std::forward<_PType>(priority), std::forward<_Function>(function));
        lock.unlock();
        this->m_ConditionVariable.notify_one();
    }
    std::mutex m_Mutex;
    bool m_Stop = false;
    std::vector<std::thread> m_Threads;
    std::priority_queue<_Work> m_Queue;
    std::condition_variable m_ConditionVariable;
};
