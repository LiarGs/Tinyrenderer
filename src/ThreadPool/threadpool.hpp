#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    ThreadPool(size_t);

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args)
        // 可以使用std::future<std::invoke_result_t<F, Args...>>;
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop; // 是否要关闭当前线程池
};

inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    auto work = [this]
    {
        while (true)
        {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock,
                                     [this]
                                     { return this->stop || !this->tasks.empty(); });
                if (this->stop && this->tasks.empty())
                    return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }

            task();
        }
    };
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(work);
}

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    using return_type = typename std::invoke_result<F, Args...>::type;

    // 获得一个指向被包装为return_type的task的智能指针,使用智能指针是为了延长task的生命周期
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::lock_guard<std::mutex> lock(queue_mutex);

        if (stop) // 关闭线程池后不允许添加任务
            throw std::runtime_error("enqueue on stopped ThreadPool");

        // 通过 lambda 表达式，将 task（std::packaged_task<return_type()>）封装为一个 std::function<void()>
        // 使其可以存储在 tasks 中
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    
    return res;
}

inline ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    // 通知所有阻塞中的线程
    condition.notify_all();
    // 析构时要保证所有线程都完成了
    for (std::thread &worker : workers)
        worker.join();
}

#endif