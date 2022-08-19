#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// c++
#include <algorithm>
#include <queue>
#include <set>
#include <vector>
#include <iostream>

// Linux
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "common.h"

using namespace std;


void bindCPU(int idx);
void printTimestamp(int printNo, const time_t& right_now);

class MessageWithSpinloc {
public:
    MessageWithSpinloc();
    Message* getMsg();
    int getLockVal();
    void setLockVal(int val);
public:
    int lock_val_;
    char msg_[8192];
};

class ShmQue {
public:
    ShmQue(int key, int cap);
    MessageWithSpinloc* getBack();
    MessageWithSpinloc* getFront();
    void popFront();
    ~ShmQue();
private:
    bool isEmpty();
    bool isFull();

public:
    int* front_;
    int* rear_;
    int* size_; // cap + 1
    MessageWithSpinloc* buffers_;
    int shmid_;
};

ShmQue shmQue(1111, 2);

inline MessageWithSpinloc::MessageWithSpinloc() : lock_val_(0) {}

inline Message* MessageWithSpinloc::getMsg() {
    return (Message*)(msg_);
}
inline int MessageWithSpinloc::getLockVal() {
    return lock_val_;
}
inline void MessageWithSpinloc::setLockVal(int val) {
    lock_val_ = val;
}

inline ShmQue::ShmQue(int key, int cap) {
    int shm_buf_size = sizeof(MessageWithSpinloc) * (cap + 1) + sizeof(int) * 3;
    shmid_ = shmget(key, shm_buf_size, IPC_CREAT | 0600);
    void* total_buf = shmat(shmid_, NULL, 0);
    front_ =  (int*)( (char*)total_buf + sizeof(int) * 0 );
    rear_  =  (int*)( (char*)total_buf + sizeof(int) * 1 );
    size_  =  (int*)( (char*)total_buf + sizeof(int) * 2 );
    *size_ = cap + 1;
    buffers_ = (MessageWithSpinloc *)((char*)total_buf + sizeof(int*) * 3);
}


inline MessageWithSpinloc* ShmQue::getBack() {
    if (isFull()) return nullptr;
    MessageWithSpinloc* ans = &buffers_[*rear_];
    *rear_ = (*rear_ + 1) % *size_;
    return ans;
}
inline void ShmQue::popFront() {
    if (!isEmpty()) {
        *front_ = (*front_ + 1) % *size_;
    }
}
inline MessageWithSpinloc* ShmQue::getFront() {
    if (!isEmpty()) {
        return &buffers_[*front_];
    }
    return nullptr;
}

inline ShmQue::~ShmQue() {
    shmctl(shmid_, IPC_RMID, NULL);
}
inline bool ShmQue::isEmpty() {
    if (*front_ == *rear_) return true;
    return false;
}

inline bool ShmQue::isFull() {
    if ((*rear_ + 1) % *size_ == *front_) return true;
    return false;
}

cpu_set_t mask;
inline void bindCPU(int idx) {
    CPU_ZERO(&mask);
    CPU_SET(idx, &mask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
}

