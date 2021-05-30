#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main() {
    int pipe1[2], pipe2[2];

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("could not create pipes.");
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
            char msg[] = "child 1 to child 2";
            write(pipe1[1], msg, sizeof(msg));
            if (read(pipe2[0], buf, sizeof(buf)) > 0) {
                printf("%d's child1(%d) received: %s\n", getppid(), getpid(), buf);
            }
            exit(0);
        } else if (i == 1) {
            close(pipe2[0]);
            close(pipe1[1]);
            if (read(pipe1[0], buf, sizeof(buf)) > 0) {
                printf("%d's child2(%d) received: %s\n", getppid(), getpid(), buf);
            }
            char msg[] = "child 2 to child 1";
            write(pipe2[1], msg, sizeof(msg));
            exit(0);
        }
    } else {
        wait(NULL);
        wait(NULL);
    }
    return 0;
}