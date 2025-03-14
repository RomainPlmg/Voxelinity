#include "ThreadPool.h"

#include "pch.h"

/*static*/ ThreadPool* s_ThreadPoolInst = nullptr;

ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_Workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    m_Condition.wait(lock, [this] { return m_Stop || !m_Tasks.empty(); });

                    if (m_Stop && m_Tasks.empty()) return;  // Exit thread loop if stopping

                    task = std::move(m_Tasks.front());
                    m_Tasks.pop();
                }
                task();  // Execute the task outside the lock
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        m_Stop = true;
    }
    m_Condition.notify_all();  // Wake up all threads to let them exit

    for (std::thread& worker : m_Workers) {
        if (worker.joinable()) {
            worker.join();  // Wait for each thread to finish
        }
    }
}

void ThreadPool::Init(size_t numThreads) { s_ThreadPoolInst = new ThreadPool(numThreads); }

void ThreadPool::Shutdown() {
    delete s_ThreadPoolInst;
    s_ThreadPoolInst = nullptr;
}

ThreadPool& ThreadPool::Get() {
    assert(s_ThreadPoolInst != nullptr);
    return *s_ThreadPoolInst;
}