#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <strings.h>
#include <time.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <iostream>
#include "ShmQue.h"

using threadFunc = void* (*) (void*);


class threadPool{
public:
	ShmQue* que;
	pthread_cond_t cond;	
	pthread_t* thread_pids;		//线程id 
	threadFunc thread_func_;		//线程处理函数 
	int thread_num;
	bool start;
  threadPool(int capacity, threadFunc func) {
    que = new ShmQue(1111, capacity);
    pthread_cond_init(&cond, NULL);        //初始化条件变量（同步）

    thread_pids = (pthread_t*)calloc(capacity, sizeof(pthread_t));
    thread_func_ = func;
    thread_num = capacity;
    start = false;
  }
  void Start() {
    if (false == start) {
      for (int i = 0; i < thread_num; i++) {
        pthread_create(&thread_pids[i], NULL, 
        thread_func_, 
        this);
        // cout << "create thread done" << endl;
      }
      start = true;
    } else {
      printf("threadPool has start\n");
    }
  }
};



#endif
