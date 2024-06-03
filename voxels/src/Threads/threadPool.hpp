#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <memory>
#include <thread>
#include <future>
#include <queue>
#include <deque>
#include <mutex>
#include <condition_variable>


namespace vlibs {

    namespace {
        const unsigned int MAX_RETURNS = 10000;
        const unsigned int MAX_THREADS = std::thread::hardware_concurrency();
        unsigned int ACTIVE_THREADS = 0;
    }

    template<typename Fq>
    class homogeneous_thread_pool {
    public:

        homogeneous_thread_pool(unsigned int ThreadCount = std::max((int)std::thread::hardware_concurrency() - (int)ACTIVE_THREADS, 0), const unsigned int _max_returns = MAX_RETURNS) {
            if (ACTIVE_THREADS + ThreadCount > MAX_THREADS) {
                ThreadCount = MAX_THREADS - ACTIVE_THREADS;
            }

            max_returns = _max_returns;

            ACTIVE_THREADS += ThreadCount;
            {
                std::scoped_lock _lock(tasks_mutex);
                pool_activated = true;
            }
            for (unsigned int i = 0; i < ThreadCount; i++) {
                threads.push_back(std::thread(&homogeneous_thread_pool<Fq>::thread_body, this));
            }
        }


        ~homogeneous_thread_pool() {
            wait_for_tasks();
            {
                std::scoped_lock _lock(tasks_mutex);
                pool_activated = false;
            }
            task_available_cv.notify_all();
            for (int i = 0; i < (int)threads.size(); i++) {
                threads[i].join();
            }
        }

        homogeneous_thread_pool(const homogeneous_thread_pool&); // non construction-copyable
        homogeneous_thread_pool& operator=(const homogeneous_thread_pool&); // non copyable


        template<typename F, typename ...A>
        void push_task(F&& task, A&&... args) {
            {
                const std::scoped_lock _lock(tasks_mutex);
                tasks.push(std::bind(std::forward<F>(task), std::forward<A>(args)...));
            }
            task_available_cv.notify_all();
        }


        void wait_for_tasks() {
            std::unique_lock _lock(tasks_mutex);
            waiting_for_tasks = true;
            task_waiting_cv.wait(_lock, [this] { return tasks.empty() && !tasks_running; });
            waiting_for_tasks = false;
        }


        std::size_t get_return_count() {
            {
                std::scoped_lock _lock(tasks_mutex);
                return tasks_return.size();
            }
        }


        void get_latest_return(Fq& ret, bool &returned) {
            {
                std::scoped_lock _lock(tasks_mutex);
                if (tasks_return.size() > 0) {
                    ret = std::move(tasks_return.front());
                    tasks_return.pop_front();
                    returned = true;
                    return;
                }
            }
            returned = false;
            return;
        }


        std::size_t get_queue_size() {
            {
                std::scoped_lock _lock(tasks_mutex);
                return tasks.size();
            }
        }
    private:

        std::vector<std::thread> threads;

        std::mutex tasks_mutex;
        std::queue<std::function<Fq()>> tasks;
        std::deque<Fq> tasks_return;

        std::condition_variable task_available_cv;
        std::condition_variable task_waiting_cv;

        unsigned int max_returns = 1000;

        unsigned int tasks_running = 0;

        bool pool_activated = false;

        bool waiting_for_tasks = false;

        void thread_body() {
            std::function<Fq()> task;
            while (true) {

                std::unique_lock _lock(tasks_mutex);
                task_available_cv.wait(_lock, [this] { return !tasks.empty() || !pool_activated; });

                if (!pool_activated) {
                    break;
                }

                task = std::move(tasks.front());
                tasks.pop();

                tasks_running++;
                _lock.unlock();

                Fq ret = task();

                _lock.lock();
                tasks_running--;

                if (tasks_return.size() == max_returns) {
                    tasks_return.pop_back();
                }
                tasks_return.push_front(std::move(ret));

                if (waiting_for_tasks && !tasks_running && tasks.empty())
                    task_waiting_cv.notify_all();
            }
        }
    };

}

#endif
