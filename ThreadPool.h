#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <iostream>
#include <condition_variable>
#include <future>
 

#include <assert.h>

class ThreadPool{
public:
    typedef std::function<void()> Task_type;//任务类型

    enum taskPriorityE {LOW,MIDDLE,HIGH};//优先级
    
    typedef std::pair<taskPriorityE,Task_type> TaskPair;//任务优先级和任务类型组合的任务

    ThreadPool(int threads_size = 4);//默认线程池大小为4
    ~ThreadPool();

    
    void stop();//终止线程池
    void addTask(const Task_type&);//添加任务
    void addTask(const TaskPair&);

private:
    //禁用拷贝构造和赋值运算符
    ThreadPool(const ThreadPool&);
    const ThreadPool& operator=(const ThreadPool&);

    bool is_started() const { return m_started;}
    void start();//运行线程池

    void threadLoop();
    Task_type take();

    typedef std::vector<std::thread*> Threads_type;
        
    struct TaskPriorityCmp
    {
        bool operator()(const TaskPair& a, const TaskPair& b) const
        {
            return a.first > b.first;
        }
    };

    int m_threads_size;//线程池大小
    std::vector<std::thread*> m_threads;//线程池
    std::priority_queue<TaskPair,std::vector<TaskPair>,TaskPriorityCmp>  m_tasks;//任务队列

    std::mutex m_mutex;//STL队列不是线程安全的，因此需要结合互斥锁
    std::condition_variable m_cond;//条件变量
    bool m_started;//是否开始
};

#endif