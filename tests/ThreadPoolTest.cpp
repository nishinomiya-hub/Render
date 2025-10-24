#include <gtest/gtest.h>
#include "Utils/ThreadPool.h"

#include <atomic>
#include <chrono>
#include <vector>
#include <mutex>
#include <algorithm>

using namespace Learn;

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<ThreadPool>();
    }

    void TearDown() override {
        pool.reset();
    }

    std::unique_ptr<ThreadPool> pool;
};

// 测试线程数量初始化
TEST_F(ThreadPoolTest, InitializesCorrectThreadCount) {
    const size_t requestedThreads = 4;
    ThreadPool customPool(requestedThreads);
    ASSERT_EQ(customPool.getThreadCnt(), requestedThreads);
}

// 测试任务执行（使用正确的签名）
TEST_F(ThreadPoolTest, ExecutesTasks) {
    std::atomic<int> counter{0};
    const int tasks = 100;
    
    for (int i = 0; i < tasks; ++i) {
        // 正确使用 void(size_t) 签名
        pool->pushTask([&](size_t) { counter++; });
    }
    
    pool->waitTasksFinish();
    ASSERT_EQ(counter, tasks);
}

// 测试带参数的任务（使用正确的签名）
TEST_F(ThreadPoolTest, HandlesTaskWithArguments) {
    std::atomic<int> sum{0};
    
    // 定义符合签名的任务
    auto add = [&](int a, int b, size_t) { sum += a + b; };
    
    // 使用第一个重载
    pool->pushTask(std::function<void(size_t)>([&, a=5, b=3](size_t) { add(a, b, 0); }));
    pool->pushTask(std::function<void(size_t)>([&, a=10, b=20](size_t) { add(a, b, 0); }));
    
    pool->waitTasksFinish();
    ASSERT_EQ(sum, (5+3) + (10+20));
}

// 测试任务执行顺序（FIFO）
TEST_F(ThreadPoolTest, ExecutesTasksInOrder) {
    std::vector<int> executionOrder;
    std::mutex mutex;
    
    for (int i = 0; i < 10; ++i) {
        // 正确使用 void(size_t) 签名
        pool->pushTask([&, i](size_t) {
            std::lock_guard<std::mutex> lock(mutex);
            executionOrder.push_back(i);
        });
    }
    
    pool->waitTasksFinish();
    
    // 验证任务按提交顺序执行
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(executionOrder[i], i);
    }
}

// 测试暂停功能（使用正确的签名）
TEST_F(ThreadPoolTest, PauseResumeBehavior) {
    std::atomic<bool> taskRan{false};
    
    // 提交任务但立即暂停
    pool->paused = true;
    pool->pushTask([&](size_t) { taskRan = true; });
    
    // 等待足够时间确保任务不会执行
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_FALSE(taskRan);
    
    // 恢复后任务应执行
    pool->paused = false;
    pool->waitTasksFinish();
    ASSERT_TRUE(taskRan);
}

// 测试等待任务完成（使用正确的签名）
TEST_F(ThreadPoolTest, WaitTasksFinishBlocksUntilCompletion) {
    std::atomic<bool> taskStarted{false};
    std::atomic<bool> taskCompleted{false};
    
    // 正确使用 void(size_t) 签名
    pool->pushTask([&](size_t) {
        taskStarted = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        taskCompleted = true;
    });
    
    // 等待任务开始
    while (!taskStarted) {
        std::this_thread::yield();
    }
    
    // 此时任务已开始但未完成
    ASSERT_FALSE(taskCompleted);
    
    // 等待任务完成
    pool->waitTasksFinish();
    ASSERT_TRUE(taskCompleted);
}

// 测试析构时等待任务完成（使用正确的签名）
TEST_F(ThreadPoolTest, DestructorWaitsForTasks) {
    std::atomic<bool> taskRan{false};
    
    {
        ThreadPool localPool;
        // 正确使用 void(size_t) 签名
        localPool.pushTask([&](size_t) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            taskRan = true;
        });
        
        // 确保任务已开始执行
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }  // 析构时会等待任务完成
    
    ASSERT_TRUE(taskRan);
}

// 测试线程ID传递
TEST_F(ThreadPoolTest, PassesThreadIdToTasks) {
    std::vector<size_t> threadIds;
    std::mutex mutex;
    const int tasks = pool->getThreadCnt() * 5;
    
    for (int i = 0; i < tasks; ++i) {
        // 正确使用 void(size_t) 签名
        pool->pushTask([&](size_t id) {
            std::lock_guard<std::mutex> lock(mutex);
            threadIds.push_back(id);
        });
    }
    
    pool->waitTasksFinish();
    
    // 验证所有线程ID都在有效范围内
    for (auto id : threadIds) {
        ASSERT_LT(id, pool->getThreadCnt());
    }
}

// 测试多线程任务提交（使用正确的签名）
TEST_F(ThreadPoolTest, HandlesConcurrentTaskSubmission) {
    std::atomic<int> counter{0};
    const int tasksPerThread = 100;
    const int threadCount = 4;
    
    std::vector<std::thread> producers;
    for (int i = 0; i < threadCount; ++i) {
        producers.emplace_back([&] {
            for (int j = 0; j < tasksPerThread; ++j) {
                // 正确使用 void(size极狐Hub
                pool->pushTask([&](size_t) { counter++; });
            }
        });
    }
    
    for (auto& t : producers) {
        t.join();
    }
    
    pool->waitTasksFinish();
    ASSERT_EQ(counter, tasksPerThread * threadCount);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}