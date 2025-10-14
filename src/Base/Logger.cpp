#include "Logger.h"

namespace Learn{


void *Logger::context_ = nullptr;
LogCallback Logger::callback_ = nullptr;

LogLevel Logger::logLevel_ = LOG_LEVEL_INFO;

char Logger::buffer[LOG_BUFFER_SIZE] = {};
std::mutex Logger::mutex_;

void Logger::init(LogCallback callback, void *context) {
    Logger::callback_ = callback;
    Logger::context_ = context;
}

void Logger::setLevel(LogLevel level)
{
    Logger::logLevel_ = level;
}

void Logger::log(LogLevel level, const char *file, int line, const char *format, ...) {
    if (level < logLevel_) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);
    va_end(args);
    buffer[LOG_BUFFER_SIZE - 1] = '\0';

    if(callback_ != nullptr ){
        callback_( context_,level, buffer);
        return;
    }

    switch (level) {
#ifdef LOG_SOURCE_LINE
    case LOG_LEVEL_INFO:    fprintf(stdout, "[INFO] %s:%d: %s\n", file, line, buffer);    break;
    case LOG_LEVEL_DEBUG:   fprintf(stdout, "[DEBUG] %s:%d: %s\n", file, line, buffer);   break;
    case LOG_LEVEL_WARN: fprintf(stdout, "[WARNING] %s:%d: %s\n", file, line, buffer); break;
    case LOG_LEVEL_ERROR:   fprintf(stdout, "[ERROR] %s:%d: %s\n", file, line, buffer);   break;
#else
    case LOG_LEVEL_INFO:    fprintf(stdout, "[INFO] : %s\n", buffer);    break;
    case LOG_LEVEL_DEBUG:   fprintf(stdout, "[DEBUG] : %s\n", buffer);   break;
    case LOG_LEVEL_WARN: fprintf(stdout, "[WARNING] : %s\n", buffer); break;
    case LOG_LEVEL_ERROR:   fprintf(stderr, "[ERROR] : %s\n", buffer);   break;
#endif
    }
    fflush(stdout);
    fflush(stderr);
}
}