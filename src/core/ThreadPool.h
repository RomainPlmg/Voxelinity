#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

class ThreadPool {
   public:
    ThreadPool(size_t numThreads);  // Constructor: Initializes the thread pool with the given number of threads
    ~ThreadPool();                  // Destructor: Shuts down the thread pool and joins all threads

    static void Init(size_t numThreads);
    static void Shutdown();

    static ThreadPool& Get();

    // Add a new task to the queue
    template <class F, class... Args>
    void Enqueue(F&& f, Args&&... args);

   private:
    std::vector<std::thread> m_Workers;         // Worker threads
    std::queue<std::function<void()>> m_Tasks;  // Task queue

    std::mutex m_QueueMutex;              // Synchronization
    std::condition_variable m_Condition;  // Condition variable
    std::atomic<bool> m_Stop;             // Stop flag
};

#endif  // __THREADPOOL_H__

template <class F, class... Args>
void ThreadPool::Enqueue(F&& f, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        m_Tasks.emplace([=] { f(args...); });  // Capture parameters by copy
    }
    m_Condition.notify_one();  // Wake up one worker thread
}