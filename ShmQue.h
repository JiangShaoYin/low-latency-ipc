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
#include "sharedMemory.h"

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

static const int MsgOffset = sizeof(time_t) + sizeof(size_t) + sizeof(long);
int* getIntPointer(void* buf, int offset);

class MessageWithSpinloc {
public:
    MessageWithSpinloc();
    void addMsgByOne();
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
    bool addTask(Message* msg);
    bool pushBack(Message* msg);
    void addSize(int val);
    bool addRear(int val);
    void setQueFull();
    void printQueStatus();
    MessageWithSpinloc* popFront();
    MessageWithSpinloc* getFront();
    MessageWithSpinloc* getTailNext();
    ~ShmQue();
    bool isEmpty();
    bool isFull();

public:
    pthread_mutex_t mutex;
    int front_;
    int rear_;
    MessageWithSpinloc* shm_bufs;
private:
    int shmid_;
    int size_; // cap + 1
};

int calculateShmIdx(MessageWithSpinloc* msg, MessageWithSpinloc* base) {
    int idx = ((char*)msg - (char*)base)/ sizeof(MessageWithSpinloc);
    return idx;
}


MessageWithSpinloc::MessageWithSpinloc() : lock_val_(0) {}
void MessageWithSpinloc::addMsgByOne() {
    msg_[MsgOffset]++;
}
Message* MessageWithSpinloc::getMsg() {
    return (Message*)(msg_);
}
int MessageWithSpinloc::getLockVal() {
    return lock_val_;
}
void MessageWithSpinloc::setLockVal(int val) {
    lock_val_ = val;
}


int* getIntPointer(void* buf, int offset) { // 获取offset byte位置上int指针
 int* p = (int*)((char*)buf + offset);
 return &p[0];
}


ShmQue::ShmQue(int key, int cap):front_(0), rear_(0), size_(cap + 1) {
        int shm_buf_size = sizeof(MessageWithSpinloc) * size_;
        shmid_ = shmget(key, shm_buf_size, IPC_CREAT | 0600);
        void* total_buf = shmat(shmid_, NULL, 0);
        
        pthread_mutex_init(&mutex, NULL);  //初始化互斥锁

        shm_bufs = (MessageWithSpinloc *)total_buf;
    }

bool ShmQue::addTask(Message* msg) {
    if (isFull())  return false;
    rear_ = (rear_ + 1) % size_;
    return true;
}
bool ShmQue::pushBack(Message* msg) {
    if (isFull())  return false;
    memcpy(&shm_bufs[rear_], msg, msg->size); 
    rear_ = (rear_ + 1) % size_;
    return true;
}
bool ShmQue::addRear(int val) {
    if (isFull() ) return false;
    rear_ = (rear_ + val) % size_;
    return true;
}
void ShmQue::addSize(int val) {
    size_ += 1;
}
MessageWithSpinloc* ShmQue::popFront() {
    // cout << "popFront 1" << endl;
    bool empty = isEmpty();
    // cout << "popFront 2" << "empty= " << empty << endl;
    if (!empty) {
        MessageWithSpinloc* ans = nullptr;
        ans = &shm_bufs[front_];
        front_ = (front_ + 1) % size_;
        // cout << "popFront" << endl;
        return ans;
    }
    return nullptr;
}
MessageWithSpinloc* ShmQue::getFront() {
    if (!isEmpty()) {
        return &shm_bufs[front_];
    }
    return nullptr;
}

MessageWithSpinloc* ShmQue::getTailNext() {
    if (isFull() ) return nullptr;
    return &shm_bufs[rear_];;
}
void ShmQue::setQueFull() {
    rear_ = (front_ + size_ - 1) % size_;
}
void ShmQue::printQueStatus() {
    cout << "       front= " << front_ << ", rear= " << rear_ << ", full= " << isFull() << endl;
}
ShmQue::~ShmQue() {
    shmctl(shmid_, IPC_RMID, NULL);
}
bool ShmQue::isEmpty() {
    if (front_ == rear_) return true;
    return false;
}

bool ShmQue::isFull() {
    if ((rear_ + 1) % size_ == front_) return true;

    return false;
}