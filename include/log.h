// AsyncLogger.h
#ifndef ASYNC_LOGGER_H
#define ASYNC_LOGGER_H

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class AsyncLogger {
public:
    static AsyncLogger& getInstance();  // 单例模式获取实例
    void log(const std::string& message);  // 日志输出方法

    void start();  // 启动日志线程
    void stop();   // 停止日志线程

private:
    AsyncLogger();  // 私有构造函数
    ~AsyncLogger();  // 析构函数，确保线程安全地退出

    void writeLogs();  // 后台线程用来写日志

    std::queue<std::string> logQueue;  // 存储日志的队列
    std::thread logThread;  // 后台日志线程
    std::mutex queueMutex;  // 队列锁
    std::condition_variable cv;  // 条件变量，用于通知线程
    std::atomic<bool> running;  // 控制线程是否继续运行
};

#endif // ASYNC_LOGGER_H
