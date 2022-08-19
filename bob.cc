#include <pthread.h>
#include "ShmQue.h"


void coreActionBob() {
      while(shmQue.buffers_[*shmQue.front_].getLockVal() != 1) {}
      MessageWithSpinloc* msg_with_spinlock = &shmQue.buffers_[*shmQue.front_];
      Message* msg = msg_with_spinlock->getMsg();
      msg->payload[0]++;
      msg->checksum = crc32(msg);
      msg_with_spinlock->setLockVal(2);
}


int main() { 
  bindCPU(2);
  while (true) {
    if (shmQue.getFront()) coreActionBob();
  }
  return 0;
}