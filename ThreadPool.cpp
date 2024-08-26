#include "ThreadPool.h"

#include "Logger.h"

// 构造函数
ThreadPool::ThreadPool(size_t numThreads, ThreadPoolType type) : m_stop(false), m_type(type)
{
    for (size_t i = 0; i < numThreads; ++i) {
        Logger::instance().log("ThreadPool::ThreadPool() type:"+std::to_string(type), 3);
        m_threads.emplace_back([this] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->m_mutex);
                    this->m_condition.wait(lock, [this] {
                        return this->m_stop || !this->m_tasks.empty();
                    });

                    if (this->m_stop && this->m_tasks.empty()) {
                        return;
                    }

                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }

                task();
            }
        });
    }
}

// 析构函数
ThreadPool::~ThreadPool()
{
    Stop();
}

void ThreadPool::Stop()
{
    Logger::instance().log("ThreadPool::~ThreadPool()");
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }

    m_condition.notify_all();

    for (std::thread &thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

// 添加任务到线程池
void ThreadPool::enqueueTask(std::function<void()> task)
{
    Logger::instance().log("ThreadPool::enqueueTask");
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_stop) {
            Logger::instance().log("ThreadPool::enqueueTask-m_stop");
            return ;
        }

        m_tasks.emplace(std::move(task));
    }

    m_condition.notify_one();
}

size_t ThreadPool::size()
{
    return m_threads.size();
}