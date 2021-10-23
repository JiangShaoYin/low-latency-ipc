#include <pthread.h>
#include "threadPool.h"


void* coreActionBob(void* p) {
    threadPool* thread_pool = (threadPool*)p;
    ShmQue* task_que = thread_pool->que;

    pthread_mutex_lock(&task_que->mutex);
    MessageWithSpinloc* msg = task_que->popFront(); // 取出队头的message
    pthread_mutex_unlock(&task_que->mutex);

  while (1) {   
    int idx = calculateShmIdx(msg, task_que->shm_bufs);
    
    cout << "wait to consume, shm id= " << idx << endl;
    while(msg->getLockVal() != 1) {}
    Message* m = msg->getMsg();
    assert(m->checksum == crc32(m));
    
    m->payload[0]++;
    m->checksum = crc32(m);
    cout << "consumer 1" << endl;
    msg->setLockVal(2);
  }
    return nullptr;
}
threadPool ThreadPool(THREAD_POOL_SIZE, coreActionBob);

// CPU_SET(1, &mask);
int main() {
  ThreadPool.que->setQueFull();
  ThreadPool.Start();
  sleep(33330);
  return 0;
}