/*
 * File: thread
 * Author: Michael Franks 
 * Description: thread pooling, multitasking but for nerds
 */

#ifndef THREAD_H
#define THREAD_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>


class Thread
{
public:
    Thread();
    ~Thread();

    template <typename F>
    void enqueue(F&& f)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            task_.emplace(std::forward<F>(f));
        }
        cv_.notify_one();
    }

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> task_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_;

    void thread_(int id);
};


#endif // THREAD_H
