#pragma once
#include "ThreadPool/ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads) {
    workers.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(tasks_guard);
        stop_flag = true;
    } 
    condition.notify_all();
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(Task task) {
    {
        std::lock_guard<std::mutex> lock(tasks_guard);
        tasks.push(std::move(task));
    }
    condition.notify_one(); // this will wake up the worker_loop
}

void ThreadPool::worker_loop() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(tasks_guard);
            condition.wait(lock, [this] { return !tasks.empty() || stop_flag; });

            if (stop_flag && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }
        // now we are outside of the thread safe block
        // execute task on the current chosen thread;
        task();
    }
}