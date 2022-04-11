#include "ThreadPool.h"

using namespace std;

mutex g_mutex;

void MyFunc()
{
  for (int i = 1; i < 1000000; ++i)
  {
      std::lock_guard<std::mutex> lock(g_mutex);
      std::cout << "MyFunc() ["  << "] at thread [ " << std::this_thread::get_id() << "] output" << std::endl;
  
  }
}


int main()
{

    ThreadPool tp;
    mutex mtx;
    
    //这里只添加四次任务，因此任务队列实际大小为4
    for (int i = 0; i < 4; ++i)
    {
        tp.addTask(MyFunc);
    }

  return 0;
}