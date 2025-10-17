#include <thread>
#include <cstdarg>
#include "gtest/gtest.h"
#include "Base/Timer.h"  // 包含待测试的头文件

class TimerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前执行
    }

    void TearDown() override {
        // 在每个测试后执行
    }
};

// 测试 Timer 的基本功能
TEST_F(TimerTest, BasicTimerFunctionality) {
    Learn::Timer timer;
    
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    timer.stop();
    
    int64_t elapsed = timer.getElapsedTime();
    
    // 验证经过的时间在合理范围内（考虑系统调度误差）
    EXPECT_GE(elapsed, 45);  // 至少 45ms
    EXPECT_LE(elapsed, 150); // 最多 150ms（考虑系统负载）
}

// 测试 Timer 多次启动停止
TEST_F(TimerTest, MultipleStartStop) {
    Learn::Timer timer;
    
    // 第一次测量
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    timer.stop();
    int64_t firstElapsed = timer.getElapsedTime();
    
    // 第二次测量
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    timer.stop();
    int64_t secondElapsed = timer.getElapsedTime();
    
    EXPECT_GE(firstElapsed, 15);
    EXPECT_GE(secondElapsed, 25);
    EXPECT_GT(secondElapsed, firstElapsed); // 第二次应该比第一次长
}

// 测试 Timer 未启动时获取时间（应该为0或未定义行为，但根据实现可能是负值）
TEST_F(TimerTest, TimerWithoutStart) {
    Learn::Timer timer;
    
    // 不调用 start()，直接调用 stop() 和 getElapsedTime()
    timer.stop();
    int64_t elapsed = timer.getElapsedTime();
    
    // 这个行为可能是未定义的，但通常应该是0或很小的值
    // 我们只验证它不会崩溃
    SUCCEED();
}

// 测试 ScopedTimer 的基本功能
TEST_F(TimerTest, ScopedTimerBasic) {
    // 注意：由于 ScopedTimer 会在析构时输出日志，我们主要测试它不会崩溃
    {
        Learn::ScopedTimer timer("ScopedTimerTest");
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    } // 这里 timer 析构，输出日志
    
    SUCCEED(); // 如果执行到这里没有崩溃，测试通过
}

// 测试 ScopedTimer 的 bool 运算符
TEST_F(TimerTest, ScopedTimerBoolOperator) {
    Learn::ScopedTimer timer("BoolOperatorTest");
    
    EXPECT_TRUE(static_cast<bool>(timer));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// 测试宏定义（在 DEBUG 模式下）
TEST_F(TimerTest, MacroDefinitions) {
    // 测试 FUNCTION_TIMED 宏
    FUNCTION_TIMED("FunctionTimedTest");
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    
    // 测试 SCOPE_TIMED 宏
    SCOPE_TIMED("ScopeTimedTest") {
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    
    SUCCEED(); // 如果执行到这里没有崩溃，测试通过
}

// 测试 Timer 的时间精度
TEST_F(TimerTest, TimerPrecision) {
    Learn::Timer timer;
    
    // 测试短时间间隔
    timer.start();
    std::this_thread::sleep_for(std::chrono::microseconds(500)); // 500 微秒
    timer.stop();
    
    int64_t elapsed = timer.getElapsedTime();
    
    // 500 微秒应该测量为 0 或 1 毫秒
    EXPECT_GE(elapsed, 0);
    EXPECT_LE(elapsed, 2);
}

// 性能测试：连续多次计时
TEST_F(TimerTest, PerformanceTest) {
    const int iterations = 10;
    Learn::Timer overallTimer;
    
    overallTimer.start();
    
    for (int i = 0; i < iterations; ++i) {
        Learn::Timer iterationTimer;
        iterationTimer.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        iterationTimer.stop();
        
        int64_t elapsed = iterationTimer.getElapsedTime();
        EXPECT_GE(elapsed, 4);
        EXPECT_LE(elapsed, 20);
    }
    
    overallTimer.stop();
    int64_t totalElapsed = overallTimer.getElapsedTime();
    
    // 总时间应该大约是 5ms * 10 = 50ms
    EXPECT_GE(totalElapsed, 45);
    EXPECT_LE(totalElapsed, 200); // 宽松的上限，考虑系统调度
}