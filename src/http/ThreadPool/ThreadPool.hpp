#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

// router will have a queue of free threads
// the threads before going to sleep will append itself to the queue
class ThreadPool {
    using Task = std::function<void()>;
public:
    // make it explicit to prevent accidental implicit conversions (e.g., ThreadPool p = 4;)
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    void enqueue(Task task);
private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex tasks_guard;
    void worker_loop();

    std::condition_variable condition;
    bool stop_flag = false;

};