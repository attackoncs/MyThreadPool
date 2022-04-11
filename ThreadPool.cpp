#include "ThreadPool.h"

ThreadPool::ThreadPool(int threads_size)
    :m_threads_size(threads_size),m_mutex(),m_cond(),m_started(false)
{
    start();
}

ThreadPool::~ThreadPool()
{
    if(m_started)
    {
        stop();
    }
}

//线程池开始
void ThreadPool::start()
{
    assert(m_threads.empty());
    assert(!m_started);
    m_started = true;
    m_threads.reserve(m_threads_size);
    for(int i=0;i<m_threads_size;++i)
    {
        m_threads.push_back(new std::thread(std::bind(&ThreadPool::threadLoop,this)));
    }
}

//线程池终止
void ThreadPool::stop()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_started = false;
    m_cond.notify_all();

    for(auto it= m_threads.begin();it!=m_threads.end();++it)
    {
        (*it)->join();
        delete *it;
    }
    m_threads.clear();
}

//线程池循环
void ThreadPool::threadLoop()
{
    while(m_started)
    {
        Task_type task = take();
        if(task)
            task();
    }
}

//添加任务
void ThreadPool::addTask(const Task_type& task)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    TaskPair taskPair(MIDDLE,task);
    m_tasks.emplace(taskPair);
    m_cond.notify_one();
}

void ThreadPool::addTask(const TaskPair& taskPair)
{

    std::unique_lock<std::mutex> lock(m_mutex);
    m_tasks.emplace(taskPair);
    m_cond.notify_one();
}

//从任务队列拿任务
ThreadPool::Task_type ThreadPool::take()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while(m_tasks.empty()&&m_started)
    {
        m_cond.wait(lock);
    }

    Task_type task;

    int size = m_tasks.size();
    if(!m_tasks.empty()&&m_started)
    {
        task = m_tasks.top().second;
        m_tasks.pop();
        assert(size -1 == m_tasks.size());
    }
    return task;
}

