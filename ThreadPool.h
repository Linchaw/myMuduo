#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

enum ThreadPoolType {
    IOThreadPool,
    WorkThreadPool
};

class ThreadPool
{
private:
    // 线程池中的工作线程
    std::vector<std::thread> m_threads;

    // 任务队列
    std::queue<std::function<void()>> m_tasks;

    // 互斥锁用于保护任务队列
    std::mutex m_mutex;

    // 条件变量用于通知线程任务的到来
    std::condition_variable m_condition;

    // 用于停止线程的标志
    bool m_stop;

    ThreadPoolType m_type;

public:
    explicit ThreadPool(size_t numThreads, ThreadPoolType type=IOThreadPool);
    ~ThreadPool();
    void Stop();

    // 添加任务到线程池
    void enqueueTask(std::function<void()> task);
    size_t size();
};

#endif // THREAD_POOL_H
