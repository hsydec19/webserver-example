// AsyncLogger.cpp
#include "log.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>  // 引入文件流

AsyncLogger::AsyncLogger() : running(false) {}

AsyncLogger::~AsyncLogger() {
    stop();  // 确保在析构时停止日志线程
}

AsyncLogger& AsyncLogger::getInstance() {
    static AsyncLogger instance;  // 返回唯一实例
    return instance;
}

void AsyncLogger::start() {
    if (!running) {
        running = true;
        logThread = std::thread(&AsyncLogger::writeLogs, this);  // 启动日志线程
    }
}

void AsyncLogger::stop() {
    if (running) {
        running = false;
        cv.notify_all();  // 通知日志线程停止
        if (logThread.joinable()) {
            logThread.join();  // 等待日志线程退出
        }
    }
}

void AsyncLogger::log(const std::string& message) {
    if (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push(message);  // 将日志信息加入队列
        cv.notify_one();  // 通知日志线程处理日志
    }
}

void AsyncLogger::writeLogs() {
    // 打开文件
    std::ofstream logFile("./log/log.txt", std::ios::app);  // 以追加模式打开文件
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file!" << std::endl;
        return;
    }

    while (running) {
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [this] { return !logQueue.empty() || !running; });  // 等待队列中的日志或停止信号

        while (!logQueue.empty()) {
            std::string logMessage = logQueue.front();
            logQueue.pop();
            lock.unlock();  // 释放锁，避免阻塞其他日志记录

            // 写入日志到文件
            logFile << logMessage << std::endl;

            lock.lock();  // 重新锁住队列
        }
    }

    logFile.close();  // 在退出前关闭文件
}
