#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>

class ThreadPool{
public:
    ThreadPool(int numThreads):m_isStopped(false){
        for(int i = 0; i < numThreads; i++){
            m_threads.emplace_back([this]{
                while(true){
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_condition.wait(lock, [this]{return !m_tasks.empty() || m_isStopped;});
                    if(m_isStopped && m_tasks.empty()){
                        return;
                    }
                    std::function<void()> task = std::move(m_tasks.front());
                    m_tasks.pop();
                    lock.unlock();
                    task();
                }
            });
        }
    }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_isStopped = true;
        }
        m_condition.notify_all();
        for(auto& thread : m_threads){
            thread.join();
        }
    }

    template<typename F, typename... Args>
    void AddTask(F &&f, Args&&... args){
        std::function<void()> task = 
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_tasks.emplace(std::move(task));
        }
        m_condition.notify_one();
    }
private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_isStopped;
};

#endif