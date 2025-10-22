#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <stdexcept>

namespace Learn {

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCnt = std::thread::hardware_concurrency())
        : running_(true),
          threadCnt_(threadCnt),
          threads_(std::make_unique<std::thread[]>(threadCnt)) {
        createThreads();
    }

    ~ThreadPool() {
        shutdown();
    }

    void shutdown() {
        if (!running_) return;
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        
        cv_.notify_all();
        taskFinishedCv_.notify_all();
        joinThreads();
    }

    inline size_t getThreadCnt() const {
        return threadCnt_;
    }

    // 基础任务提交（无返回值）
    template<typename F>
    void pushTask(F&& task) {
        pushTaskImpl(std::forward<F>(task), std::false_type{});
    }

    // 带参数的任务提交（无返回值）
    template<typename F, typename... Args>
    void pushTask(F&& task, Args&&... args) {
        auto boundTask = [task = std::forward<F>(task), 
                         ...args = std::forward<Args>(args)]() mutable {
            return task(std::forward<Args>(args)...);
        };
        pushTaskImpl(std::move(boundTask), std::false_type{});
    }

    // 提交带线程ID的任务（无返回值）
    template<typename F>
    void pushTaskWithThreadId(F&& task) {
        // 检查线程池是否在运行
        if (!running_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("pushTask on stopped ThreadPool");
        }
        
        // 使用完美转发包装任务
        auto wrappedTask = [task = std::forward<F>(task)](size_t threadId) mutable {
            task(threadId);
        };
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.emplace(std::move(wrappedTask));
            tasksTotal_++;
        }
        
        cv_.notify_one();
    }

    // 提交任务并返回 future（有返回值）
    template<typename F>
    auto submitTask(F&& task) -> std::future<std::invoke_result_t<F>> {
        return pushTaskImpl(std::forward<F>(task), std::true_type{});
    }

    // 带参数的任务提交并返回 future
    template<typename F, typename... Args>
    auto submitTask(F&& task, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>> {
        using ReturnType = std::invoke_result_t<F, Args...>;
        
        auto boundTask = [task = std::forward<F>(task), 
                         ...args = std::forward<Args>(args)]() mutable -> ReturnType {
            return task(std::forward<Args>(args)...);
        };
        
        return pushTaskImpl(std::move(boundTask), std::true_type{});
    }

    void waitTasksFinish() const {
        std::unique_lock<std::mutex> lock(mutex_);
        if (paused) {
            // 等待所有运行中的任务完成
            taskFinishedCv_.wait(lock, [this] {
                return tasksRunning_ == 0;
            });
        } else {
            // 等待所有任务完成
            taskFinishedCv_.wait(lock, [this] {
                return tasks_.empty() && tasksRunning_ == 0;
            });
        }
    }

    std::atomic<bool> paused{false};

private:
    void createThreads() {
        for (size_t i = 0; i < threadCnt_; i++) {
            threads_[i] = std::thread(&ThreadPool::taskWorker, this, i);
        }
    }

    void joinThreads() {
        for (size_t i = 0; i < threadCnt_; i++) {
            if (threads_[i].joinable()) {
                threads_[i].join();
            }
        }
    }

    template<typename F, typename HasReturn>
    auto pushTaskImpl(F&& task, HasReturn) 
        -> std::conditional_t<HasReturn::value, 
                              std::future<std::invoke_result_t<F>>, 
                              void> 
    {
        using TaskReturnType = std::invoke_result_t<F>;
        
        // 检查线程池是否在运行
        if (!running_.load(std::memory_order_relaxed)) {
            throw std::runtime_error("pushTask on stopped ThreadPool");
        }
        
        // 处理有返回值的任务
        if constexpr (HasReturn::value) {
            auto packagedTask = std::make_shared<std::packaged_task<TaskReturnType()>>(
                [task = std::forward<F>(task)]() mutable {
                    return task();
                }
            );
            
            std::future<TaskReturnType> future = packagedTask->get_future();
            
            {
                std::lock_guard<std::mutex> lock(mutex_);
                tasks_.emplace([packagedTask]() {
                    (*packagedTask)();
                });
                tasksTotal_++;
            }
            
            cv_.notify_one();
            return future;
        } 
        // 处理无返回值的任务
        else {
            // 使用完美转发包装任务
            auto wrappedTask = [task = std::forward<F>(task)]() mutable {
                task();
            };
            
            {
                std::lock_guard<std::mutex> lock(mutex_);
                tasks_.emplace(std::move(wrappedTask));
                tasksTotal_++;
            }
            
            cv_.notify_one();
        }
    }

    void taskWorker(size_t threadId) {
        while (true) {
            std::function<void(size_t)> task;
            
            {
                std::unique_lock<std::mutex> lock(mutex_);
                
                // 等待任务或停止信号
                cv_.wait(lock, [this] {
                    return !running_ || (!paused && !tasks_.empty());
                });
                
                // 检查是否需要退出
                if (!running_ && (paused || tasks_.empty())) {
                    return;
                }
                
                // 如果暂停或没有任务，继续等待
                if (paused || tasks_.empty()) {
                    continue;
                }
                
                // 获取任务
                task = std::move(tasks_.front());
                tasks_.pop();
                tasksRunning_++;
            }
            
            // 执行任务，传递线程ID
            try {
                task(threadId);
            } catch (...) {
                // 异常处理（可根据需要记录日志）
            }
            
            {
                std::lock_guard<std::mutex> lock(mutex_);
                tasksRunning_--;
                tasksTotal_--;
                
                // 通知等待任务完成的线程
                taskFinishedCv_.notify_all();
            }
        }
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    mutable std::condition_variable taskFinishedCv_;
    
    std::atomic<bool> running_{true};
    std::unique_ptr<std::thread[]> threads_;
    std::atomic<size_t> threadCnt_{0};

    // 任务队列：每个任务接受一个 size_t 参数（线程ID）
    std::queue<std::function<void(size_t)>> tasks_;
    std::atomic<size_t> tasksTotal_{0};
    std::atomic<size_t> tasksRunning_{0};
};

}