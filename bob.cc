#include <pthread.h>
#include "threadPool.h"


void* coreActionBob(void* p) {
    threadPool* thread_pool = (threadPool*)p;
    ShmQue* task_que = thread_pool->que;

    pthread_mutex_lock(&task_que->mutex);
    MessageWithSpinloc* msg = task_que->popFront(); // 取出队头的message
    pthread_mutex_unlock(&task_que->mutex);
    auto pid = getppid();
    while (1) {   
      int idx = calculateShmIdx(msg, task_que->shm_bufs);
      // cout << "thread ppid= " << pid << ", wait to consume, shm id= " << idx; task_que->printQueStatus(); 
      while(msg->getLockVal() != 1) {}
      Message* m = msg->getMsg();
      assert(m->checksum == crc32(m));
      
      m->payload[0]++;
      m->checksum = crc32(m);
      // cout << "thread ppid= " << pid << ", set 2"; task_que->printQueStatus(); 
      msg->setLockVal(2);
    }
    return nullptr;
}
threadPool ThreadPool(SHM_CAPACITY, coreActionBob);

// CPU_SET(1, &mask);
int main() {
  ThreadPool.que->addSize(1); // size = 2 + 1 = 3;
  ThreadPool.que->setQueFull();
  ThreadPool.Start();
  sleep(33330);
  return 0;
}