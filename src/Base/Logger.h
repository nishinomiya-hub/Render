#pragma once

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <mutex>

namespace Learn{

#define LOG_INFO(...) Logger::log(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) Logger::log(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

    
static constexpr int LOG_BUFFER_SIZE = 1024;

typedef void (*LogCallback)(void *context, int level , const char *msg );

enum LogLevel{
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
};

class Logger{
public:
    static void init(LogCallback callback, void *context);
    static void setLevel(LogLevel level);
    static void log(LogLevel level, const char *file, int line, const char *format, ...);

private:
    static LogCallback callback_;
    static void *context_;
    static LogLevel logLevel_;

    static char buffer[LOG_BUFFER_SIZE];
    static std::mutex mutex_;
};

}