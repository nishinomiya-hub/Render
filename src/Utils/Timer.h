#pragma once

#include <chrono>
#include <string>

#include "Logger.h"

namespace Learn{


    //基础计时器，记录时间间隔
class Timer {
public:

    void start() {
        m_startTime = std::chrono::high_resolution_clock::now();
    }
    void stop() {
        m_stopTime = std::chrono::high_resolution_clock::now();
    }
    //可移植性
    int64_t getElapsedTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(m_stopTime - m_startTime).count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_stopTime;
};

//基于 RAII 的作用域计时器，自动记录代码块耗时并输出日志
class ScopedTimer {
public:
    ScopedTimer( const char *str)
        : m_name(str)
    {
        m_timer.start();
    }

    ~ScopedTimer()
    {
        m_timer.stop();
        LOG_DEBUG("[%s] took %lld ms", m_name.c_str(), m_timer.getElapsedTime());
    }

    explicit operator bool() {
        return true;
    }
private:
    Timer m_timer;
    std::string m_name;
};


//为测试函数性能
#ifndef NDEBUG
#   define FUNCTION_TIMED(X) Learn::ScopedTimer _functionTimer = (X)
#   define SCOPE_TIMED(X) if (Learn::ScopedTimer _scopeTimer = (X))
#else
#   define FUNCTION_TIMED(X)
#   define SCOPE_TIMED(X)
#endif
}