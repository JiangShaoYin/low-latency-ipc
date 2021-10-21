#include "common.h"
#include "lock/sem.h"
#include <pthread.h>
#include "lock/mtx.h"


void coreActionBob() {
  while (true) {
    while(spinLock.get() != 1) {}
    assert(((Message*)(shm.buf))->checksum == crc32((Message*)(shm.buf)));
    ((Message*)(shm.buf))->payload[0]++;          // 第一个字符加一
    ((Message*)(shm.buf))->checksum = crc32(((Message*)(shm.buf)));  // 更新校验和
    spinLock.set(2);
  }

}
int main() {
  coreActionBob();
  return 0;
}