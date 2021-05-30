#include <cstring>
#include <fstream>
#include <iostream>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

union semun {
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

void sem_init(int semid, unsigned short semnum);
void sem_P(int semid, unsigned short semnum);
void sem_V(int semid, unsigned short semnum);
int sem_get(int semid, unsigned short semnum);

const int BUF_SIZE = 64 * 1024;
const int DATA_SIZE = 32 * 1024;

char data[DATA_SIZE];

union semun arg;

void write_memory(key_t key, int semid) {
    int shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("could not shmget.");
        exit(-1);
    }

    char *shmadd = static_cast<char *>(shmat(shmid, NULL, 0));

    printf("%d's child(%d) copy data to shared-memory...\n", getppid(),
           getpid());

    memset(shmadd, 0, BUF_SIZE);

    ifstream in("data.txt", ios::in);

    in.read(data, DATA_SIZE);

    for (int i = 0; i < DATA_SIZE; i++) {
        sem_P(semid, 0);
        memcpy(shmadd + i, data + i, 1);
        usleep(500);
        sem_V(semid, 1);
    }

    if (shmdt(shmadd) < 0) {
        perror("could not shmdt.");
    }
}

void read_memory(key_t key, int semid) {
    int shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("could not shmget.");
        exit(-1);
    }

    char *shmadd = static_cast<char *>(shmat(shmid, NULL, 0));

    for (int i = 0; i < DATA_SIZE; i++) {
        sem_P(semid, 1);
        printf("%c", shmadd[i]);
        usleep(500);
        sem_V(semid, 0);
    }

    if (shmdt(shmadd) < 0) {
        perror("could not shmdt.");
    }
}

int main() {
    key_t key = ftok("../", 2021);
    if (key == -1) {
        perror("could not ftok.");
    }

    int shmid = shmget(key, BUF_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("could not shmget.");
    }

    int semid = semget(key, 2, IPC_CREAT | 0600);
    if (semid < 0) {
        perror("could not create semid");
        exit(-1);
    }
    arg.val = 1;
    sem_init(semid, 0);
    arg.val = 0;
    sem_init(semid, 1);

    pid_t pid;
    int i;
    for (i = 0; i < 2; i++) {
        pid = fork();
        if (pid == 0 || pid == -1) {
            break;
        }
    }

    if (pid < 0) {
        perror("could not fork.");
    } else if (pid == 0) {
        if (i == 0) {
            write_memory(key, semid);
            exit(0);
        } else if (i == 1) {
            read_memory(key, semid);
            exit(0);
        }
    } else {
        wait(NULL);
        wait(NULL);
    }

    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID, arg);
    semctl(semid, 1, IPC_RMID, arg);
    return 0;
}

void sem_init(int semid, unsigned short semnum) {
    semctl(semid, semnum, SETVAL, arg);
}
void sem_P(int semid, unsigned short semnum) {
    struct sembuf sops = {semnum, -1, SEM_UNDO};
    semop(semid, &sops, 1);
}

void sem_V(int semid, unsigned short semnum) {
    struct sembuf sops = {semnum, +1, SEM_UNDO};
    semop(semid, &sops, 1);
}

int sem_get(int semid, unsigned short semnum) {
    return semctl(semid, semnum, GETVAL, arg);
}