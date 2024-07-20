#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include "Common/cpplang.hpp"

BEGIN_NAMESPACE(GLBase)

class ThreadPool
{
public:
    explicit ThreadPool(const size_t threadCount = std::thread::hardware_concurrency())
        : m_threadCount(threadCount), m_threads(new std::thread[threadCount])
    {
        createThread();
    }

    ~ThreadPool()
    {
        waitTasksFinish();
        m_running = false;
        joinThreads();
    }

public:
    size_t getThreadCount() const
    {
        return m_threadCount;
    }

    template<typename F>
    void pushTask(const F &task)
    {
        m_tasksCount++;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_tasks.push(std::function<void(size_t)>(task));
        }
    }

    template<typename F, typename... A>
    void pushTask(const F &task, const A &... args)
    {
        pushTask([task, args...]{task(args...);});
    }

    void waitTasksFinish() const
    {
        while (true)
        {
            if (!paused)
            {
                if (m_tasksCount == 0)
                    break;
            }
            else
            {
                if (tasksRunningCount() == 0)
                    break;
            }
            std::this_thread::yield();
        }
    }

    std::atomic<bool> paused{false};

private:
    void createThread()
    {
        for(size_t i = 0; i < m_threadCount; i++)
        {
            m_threads[i] = std::thread(&ThreadPool::taskWorker, this, i);
        }
    }

    void joinThreads()
    {
        for(size_t i = 0; i < m_threadCount; i++)
        {
            m_threads[i].join();
        }
    }

    size_t tasksQueuedCount() const
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

    size_t tasksRunningCount() const
    {
        return m_tasksCount - tasksQueuedCount();
    }

    bool popTask(std::function<void(size_t)> &task)
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        if (m_tasks.empty())
        {
            return false;
        }
        task = std::move(m_tasks.front());
        m_tasks.pop();
        return true;
    }

    void taskWorker(size_t threadId)
    {
        while(m_running)
        {
            std::function<void(size_t)> task;
            if (!paused && popTask(task))
            {
                task(threadId);
                m_tasksCount--;
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

private:
    mutable std::mutex m_mutex{};
    std::atomic<bool> m_running{true};
    std::unique_ptr<std::thread[]> m_threads;
    std::atomic<size_t> m_threadCount{0};
    std::queue<std::function<void(size_t)>> m_tasks{};
    std::atomic<size_t> m_tasksCount{0};
};

END_NAMESPACE(GLBase)

#endif // _THREAD_POOL_HPP_