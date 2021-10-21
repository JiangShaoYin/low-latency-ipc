#include <sys/ipc.h>
#include <sys/shm.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>


class ShmMutex {
public:
    pthread_mutex_t *m;
    ShmMutex(int key) {
        shmid = shmget(key, sizeof(pthread_mutex_t), IPC_CREAT | 0666);
        m = (pthread_mutex_t*)shmat(shmid, NULL, 0);
        pthread_mutexattr_init(&mutextattr);
        pthread_mutexattr_setpshared(&mutextattr, PTHREAD_PROCESS_SHARED);
        pthread_mutexattr_setrobust(&mutextattr, PTHREAD_MUTEX_ROBUST);
        pthread_mutex_init(m, &mutextattr);
    }
    void set(pthread_mutex_t& mutex) {
        // memcpy(m, &mutex, sizeof(mutex));
    }
    ~ShmMutex() {
        pthread_mutexattr_destroy(&mutextattr);
        pthread_mutex_destroy(m);
        shmctl(shmid, IPC_RMID, NULL);
    }
private:
    int shmid;
    pthread_mutexattr_t mutextattr;
};

class ShmCond {
public:
    pthread_cond_t *cond;
    ShmCond(int key) {
        shmid = shmget(key, sizeof(pthread_cond_t), IPC_CREAT | 0666);
        cond = (pthread_cond_t*)shmat(shmid, NULL, 0);

        pthread_condattr_init(&condAttr);
        pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(cond, &condAttr);
    }

    ~ShmCond() {
        shmctl(shmid, IPC_RMID, NULL);
        pthread_condattr_destroy(&condAttr);
        pthread_cond_destroy(cond);
    }
private:
    int shmid;
    pthread_condattr_t condAttr;
};


