#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2];

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0 || pipe(pipe3) < 0 ||
        pipe(pipe4) < 0) {
        perror("could not create pipes.");
        exit(-1);
    }

    pid_t pid;
    int i;
    for (i = 0; i < 2; i++) {
        pid = fork();
        if (pid == 0 || pid == -1) {
            break;
        }
    }
    char buf[256];

    if (pid < 0) {
        perror("could not fork().");
    } else if (pid == 0) {
        if (i == 0) {
            close(pipe1[0]);
            close(pipe2[1]);
            close(pipe3[0]);
            close(pipe3[1]);
            close(pipe4[0]);
            close(pipe4[1]);
            char msg[] = "child 1 to child 2";
            for (int i = 0; i < 10; i++) {
                write(pipe1[1], msg, sizeof(msg));
                if (read(pipe2[0], buf, sizeof(buf)) > 0) {
                    printf("%d's child 1(%d) received: %s\n", getppid(),
                           getpid(), buf);
                }
                sleep(1);
            }
            exit(0);
        } else if (i == 1) {
            close(pipe1[0]);
            close(pipe1[1]);
            close(pipe2[0]);
            close(pipe2[1]);
            close(pipe4[0]);
            close(pipe3[1]);
            for (int i = 0; i < 10; i++) {
                if (read(pipe3[0], buf, sizeof(buf)) > 0) {
                    printf("%d's child 2(%d) received: %s\n", getppid(),
                           getpid(), buf);
                }
                char msg[] = "child 2 to child 1";
                write(pipe4[1], msg, sizeof(msg));
                sleep(1);
            }
            exit(0);
        }
    } else {
        close(pipe1[1]);
        close(pipe3[0]);
        close(pipe2[0]);
        close(pipe4[1]);

        while (true) {
            read(pipe1[0], buf, sizeof buf);
            write(pipe3[1], buf, sizeof buf);
            read(pipe4[0], buf, sizeof buf);
            write(pipe2[1], buf, sizeof buf);
        }

        wait(NULL);
        wait(NULL);
    }
    return 0;
}