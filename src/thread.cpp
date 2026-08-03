/*
 * File: thread
 * Author: Michael Franks 
 * Description:
 */

#include "thread.h"

namespace count
{
    static size_t default_thread_count() {
        unsigned int hw = std::thread::hardware_concurrency();
        return hw == 0 ? 4 : hw;
    }
}

Thread::Thread() : stop_(false)
{
    for (size_t i = 0; i < count::default_thread_count(); ++i)
    {
        threads_.emplace_back([this, i]
        {
            // creation of worker threads
            thread_(i);
        });
    }
}

Thread::~Thread()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    cv_.notify_all();

    for (auto& t : threads_)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
}

void Thread::thread_(int id)
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            cv_.wait(lock, [this]
            {
                return stop_ || !task_.empty();
            });

            if (stop_ && task_.empty())
            {
                break;
            }

            if (!task_.empty())
            {
                task = std::move(task_.front());
                task_.pop();
            }
        }
        if (task)
        {
            task();
        }
    }
}
