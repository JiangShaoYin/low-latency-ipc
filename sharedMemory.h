// #include <sys/ipc.h>
// #include <sys/shm.h>
// #include <stddef.h>
// #include <errno.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <sys/types.h>
// #include <sys/mman.h>
// #include <sys/wait.h>
// #include <string.h>

// using namespace std;

// class ShareMem {
// public:
//     void* buf;
//     ShareMem(int key) {
//         shmid = shmget(key, 8192, IPC_CREAT | 0600);
//         buf = (void *)shmat(shmid, NULL, 0);
//     }
//     ShareMem(int key, void* addr, int size) {
//         shmid = shmget(key, size, IPC_CREAT |SHM_REMAP| 0600);
//         buf = (void *)shmat(shmid, addr, 0);
//         if (buf == (void*)-1) perror("shmat error: ");
//     }
//     ~ShareMem() {
//         shmctl(shmid, IPC_RMID, NULL);
//     }
//     int get() {
//         return *(int*)(buf);
//     }
//     void set(int val) {
//         int *p = (int*)(buf);
//         *p = val;
//     }
// private:
//     int shmid;
// };



