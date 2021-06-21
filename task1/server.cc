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

void handle_error(const string &msg) {
    cout << "Error: " << msg << endl;
    exit(-1);
}

int running_cnt = 0;

void show_running_count() {
    cout << "current running connections: " << running_cnt << '\n';
}

void childsignal_handler(int sig) {
    pid_t pid;
    int stat;

    // 处理僵尸进程
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated.\n", pid);
    running_cnt--;
    show_running_count();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: ./server <addr> <port>" << '\n';
        return 0;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        handle_error("could not create socket.");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));
    if (bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        handle_error("could not bind address.");
    }

    listen(sockfd, 20);
    printf("server is listening to %s:%s...\n", argv[1], argv[2]);

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    char buf[1024];
    
    signal(SIGCHLD, childsignal_handler);
    
    while (true) {
        int connfd = accept(
            sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
        pid_t pid = fork();
        running_cnt++;
        if (pid < 0) {
            handle_error("could not fork.");
        } else if (pid == 0) {
            read(connfd, buf, sizeof(buf));
            int time = atoi(buf);
            printf("current pid = %d, time = %ds\n", getpid(), time);
            sleep(time);
            close(connfd);
            exit(0);
        } else {
            show_running_count();
        }
    }

    close(sockfd);
    return 0;
}
