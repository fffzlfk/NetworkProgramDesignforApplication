#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

void handleError(const string &msg) {
    cout << "Error: " << msg << endl;
    exit(-1);
}

int running_cnt = 0;

void showRunningCount() {
    cout << "current running connections: " << running_cnt << '\n';
}

void childSignalHandler(int sig) {
    pid_t pid;
    int stat;

    // 处理僵尸进程
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated.\n", pid);
    running_cnt--;
    showRunningCount();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./server <addr> <port>" << '\n';
        return 0;
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        handleError("could not create socket.");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        handleError("could not bind address.");
    }

    listen(server_socket, 20);
    printf("server is listening to %s:%s...\n", argv[1], argv[2]);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    signal(SIGCHLD, childSignalHandler);

    while (true) {
        int client_socket = accept(
            server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        char buf[1024];
        pid_t pid = fork();
        running_cnt++;
        if (pid < 0) {
            handleError("could not fork.");
        } else if (pid == 0) {
            read(client_socket, buf, sizeof(buf));
            int time = atoi(buf);
            printf("current pid = %d, time = %ds\n", getpid(), time);
            sleep(time);
            close(client_socket);
            exit(0);
        } else {
            showRunningCount();
        }
    }

    close(server_socket);
    return 0;
}