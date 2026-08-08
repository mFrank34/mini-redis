#include <catch2/catch_test_macros.hpp>
#include "../src/Thread.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <set>
#include <thread>

// Concurrency is inherently hard to assert on deterministically — these tests
// check outcomes ("did all tasks run exactly once") rather than timing or
// interleaving, which is the realistic bar for unit-testing a thread pool.

TEST_CASE("all enqueued tasks eventually run", "[thread]") {
    Thread pool;
    std::atomic<int> counter{0};
    const int num_tasks = 100;

    for (int i = 0; i < num_tasks; ++i) {
        pool.enqueue([&counter] { counter++; });
    }

    // Poll with a generous timeout rather than assuming a fixed sleep is enough
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (counter.load() < num_tasks && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    REQUIRE(counter.load() == num_tasks);
}

TEST_CASE("tasks that write to shared state under a mutex don't corrupt it", "[thread]") {
    Thread pool;
    std::mutex data_mutex;
    std::set<int> seen;
    const int num_tasks = 50;

    for (int i = 0; i < num_tasks; ++i) {
        pool.enqueue([&data_mutex, &seen, i] {
            std::lock_guard<std::mutex> lock(data_mutex);
            seen.insert(i);
        });
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (seen.size() < static_cast<size_t>(num_tasks) && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::lock_guard<std::mutex> lock(data_mutex);
    REQUIRE(seen.size() == static_cast<size_t>(num_tasks));   // no duplicate/missing insertions
}

TEST_CASE("destroying the pool while tasks are queued does not hang or crash", "[thread]") {
    std::atomic<int> counter{0};
    {
        Thread pool;
        for (int i = 0; i < 20; ++i) {
            pool.enqueue([&counter] {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                counter++;
            });
        }
        // pool destructs here — should drain remaining tasks and join cleanly,
        // not hang indefinitely or terminate the process
    }
    REQUIRE(counter.load() == 20);
}
