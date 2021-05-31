#include <WinSock2.h>

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    string addr = "127.0.0.1";
    int port = 8080;
    int time = 10;
    if (argc == 4) {
        addr = argv[1];
        port = atoi(argv[2]);
        time = atoi(argv[3]);
    }

    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsadata;
    if (WSAStartup(sockVersion, &wsadata) != 0) {
        exit(-1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(addr.c_str());
    server_addr.sin_port = htons(port);

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) ==
        SOCKET_ERROR) {
        perror("could not connect.");
        exit(-1);
    }
    char msg[256];
    _itoa_s(time, msg, 10);
    send(client_socket, msg, sizeof(msg), 0);
    closesocket(client_socket);
    return 0;
}