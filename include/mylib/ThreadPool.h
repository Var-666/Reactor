//
// Created by Var on 25-8-13.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#pragma once

#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_num);
    ~ThreadPool();

    void submit(std::function<void()> task);   // 提交任务到线程池

    void shutdown();      // 停止线程池（等待任务完成）
private:
    void workerThread();

    std::vector<std::thread> workers_;    //工作线程
    std::queue<std::function<void()>> tasks_;   //任务队列

    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_;
};



#endif //THREADPOOL_H
