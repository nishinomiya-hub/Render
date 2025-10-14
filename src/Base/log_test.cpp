#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include "Logger.h"  // 假设头文件名为Logger.hpp

#include <WINDOWS.H>

using namespace Learn;

// 修改回调函数以显示上下文
void testCallback(void* context, int level, const char* msg) {
    const char* levelStr = "";
    switch (level) {
        case LOG_LEVEL_INFO: levelStr = "INFO"; break;
        case LOG_LEVEL_DEBUG: levelStr = "DEBUG"; break;
        case LOG_LEVEL_WARN: levelStr = "WARN"; break;
        case LOG_LEVEL_ERROR: levelStr = "ERROR"; break;
    }
    // 添加上下文显示
    const char* ctx = context ? static_cast<const char*>(context) : "全局";
    std::cout << "[回调][" << ctx << "] " << levelStr << " | " << msg << std::endl;
}

// 修复日志级别测试（添加上下文）
void testLevelFiltering() {
    Logger::init(testCallback, (void*)"级别测试");
    Logger::setLevel(LOG_LEVEL_WARN);

    LOG_INFO("这条INFO消息不应出现");
    LOG_DEBUG("这条DEBUG消息不应出现");
    LOG_WARN("警告：级别过滤测试");
    LOG_ERROR("错误：级别过滤测试");
}

// 修复日志内容测试（确保INFO级别可见）
void testLogContent() {
    const char* testContext = "内容测试";
    Logger::init(testCallback, (void*)testContext);
    Logger::setLevel(LOG_LEVEL_INFO);  // 添加这行确保INFO可见

    LOG_INFO("用户登录成功");
    LOG_ERROR("文件打开失败");
}

// 修复多线程测试（调整日志级别和ID打印）
void testMultithreadSafety() {
    Logger::init(testCallback, (void*)"线程测试");
    Logger::setLevel(LOG_LEVEL_DEBUG);  // 添加这行确保DEBUG可见
    
    auto worker = []() {
        for (int i = 0; i < 3; ++i) {
            // 修复线程ID打印方式
            std::stringstream ss;
            ss << std::this_thread::get_id();
            LOG_DEBUG("线程ID: %s", ss.str().c_str());
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(worker);
    }
    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8); // 设置输出编码为UTF-8
    SetConsoleCP(CP_UTF8);       // 设置输入编码为UTF-8

    std::cout << "===== 测试1：日志级别过滤 =====" << std::endl;
    testLevelFiltering();

    std::cout << "\n===== 测试2：日志内容和上下文 =====" << std::endl;
    testLogContent();

    std::cout << "\n===== 测试3：多线程安全 =====" << std::endl;
    testMultithreadSafety();

    return 0;
}