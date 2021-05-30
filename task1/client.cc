#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void handle_error(const string &msg) {
    cout << "Error: " << msg << endl;
    exit(-1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Usage: ./client <addr> <port> <time>" << '\n';
        return 0;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        handle_error("could not create socket.");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if ((connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr))) <
        0) {
        handle_error("could not connect to server.");
    }

    char time[256];
    strcpy(time, argv[3]);
    write(sock, time, sizeof(time));

    close(sock);
    return 0;
}