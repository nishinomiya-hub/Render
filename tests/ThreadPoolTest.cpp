#include "Base/ThreadPool.h"  // 包含你的线程池头文件
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>

namespace {

using namespace Learn;

// 测试基本任务执行
TEST(ThreadPoolTest, BasicTaskExecution) {
    ThreadPool pool(2);
    std::atomic<int> counter(0);
    
    pool.pushTask([&counter] { counter++; });
    pool.pushTask([&counter] { counter += 2; });
    
    pool.waitTasksFinish();
    ASSERT_EQ(counter.load(), 3);
}

// 测试带参数的任务
TEST(ThreadPoolTest, TaskWithArguments) {
    ThreadPool pool;
    std::atomic<int> result(0);
    
    auto add = [](int a, int b) { return a + b; };
    pool.pushTask([&result, add] { result = add(3, 4); });
    
    pool.waitTasksFinish();
    ASSERT_EQ(result.load(), 7);
}

// 测试返回值任务
TEST(ThreadPoolTest, ReturnValueTask) {
    ThreadPool pool;
    
    auto future = pool.submitTask([] { return 42; });
    ASSERT_EQ(future.get(), 42);
}

// 测试任务完成等待
TEST(ThreadPoolTest, WaitForTaskCompletion) {
    ThreadPool pool;
    std::atomic<bool> taskCompleted(false);
    
    pool.pushTask([&taskCompleted] {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        taskCompleted = true;
    });
    
    ASSERT_FALSE(taskCompleted.load());
    pool.waitTasksFinish();
    ASSERT_TRUE(taskCompleted.load());
}

// 测试暂停功能
TEST(ThreadPoolTest, PauseResume) {
    using namespace std::chrono_literals; // 添加时间字面量支持
    
    ThreadPool pool(1);
    std::atomic<int> counter(0);
    
    pool.pushTask([&counter] { counter++; });
    pool.waitTasksFinish();
    ASSERT_EQ(counter.load(), 1);
    
    pool.pause();
    pool.pushTask([&counter] { counter++; });
    
    // 使用时间字面量的等待
    auto start = std::chrono::steady_clock::now();
    while (counter.load() == 1 && 
           (std::chrono::steady_clock::now() - start) < 50ms) {
        std::this_thread::yield();
    }
    ASSERT_EQ(counter.load(), 1);
    
    pool.resume();
    pool.waitTasksFinish();
    ASSERT_EQ(counter.load(), 2);
}
// 测试关闭后拒绝新任务
TEST(ThreadPoolTest, ShutdownRejectsNewTasks) {
    ThreadPool pool;
    pool.shutdown();
    
    EXPECT_THROW(
        pool.pushTask([] {}),
        std::runtime_error
    );
}

// 测试异常处理
TEST(ThreadPoolTest, ExceptionHandling) {
    ThreadPool pool;
    bool exceptionCaught = false;
    
    try {
        auto future = pool.submitTask([] {
            throw std::runtime_error("Test error");
            return 1;
        });
        future.get();  // 这里会抛出异常
    } catch (...) {
        exceptionCaught = true;
    }
    
    ASSERT_TRUE(exceptionCaught);
}

// 测试多线程竞争
TEST(ThreadPoolTest, ConcurrentTaskSubmission) {
    ThreadPool pool(4);
    std::atomic<int> counter(0);
    constexpr int TASK_COUNT = 1000;
    
    for (int i = 0; i < TASK_COUNT; ++i) {
        pool.pushTask([&counter] { counter++; });
    }
    
    pool.waitTasksFinish();
    ASSERT_EQ(counter.load(), TASK_COUNT);
}

}  // namespace