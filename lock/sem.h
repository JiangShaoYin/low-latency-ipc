// #include <sys/ipc.h>
// #include <sys/sem.h>
// // #include <semaphore.h>




// class Sem {
// public:
//     Sem() {
//         sops[0].sem_num = 0;
//         sops[1].sem_num = 1;
//         sops[0].sem_flg = 0;
//         sops[1].sem_flg = 0;
//         unsigned short sem_array[2];
//         union semun {
//             int val;
//             struct semid_ds *buf;
//             unsigned short *array;
//         }arg;
//         sem_array[0] = 0;
//         sem_array[1] = 1;
//         arg.array = sem_array;
//         semid = semget(1234, 1, IPC_CREAT | 0600);
//         int ret = semctl(semid, 0, SETALL, arg);
//         if (ret == -1) {cout << "SETALL failed, errno= " << errno << endl;}

//     }
//     ~Sem() {
//         semctl(semid, 0, IPC_RMID);
//     }

//     void alice2bob(int op) {
//         sops[0].sem_op = op;
//         semop(semid, (sembuf*)&sops[0], 1); 
//     }
//     void bob2alice(int op) {
//         sops[1].sem_op = op;
//         semop(semid, (sembuf*)&sops[1], 1); 
//     }
// private:
//     int semid;
//     sembuf sops[2]; // 0是alice发的， 1是alice收的
// };