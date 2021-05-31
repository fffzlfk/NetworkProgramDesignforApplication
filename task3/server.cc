#include <Windows.h>
#include <Winsock2.h>

#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

wchar_t* str2wchar(string str);
void showRunning(const vector<PROCESS_INFORMATION>& pids);

int main(int argc, char* argv[]) {
    string addr = "127.0.0.1";
    int port = 8080;
    if (argc == 3) {
        addr = argv[1];
        port = atoi(argv[2]);
    }

    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsadata;
    if (WSAStartup(sockVersion, &wsadata) != 0) {
        exit(-1);
    }

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET) {
        perror("could not create socket.");
        exit(-1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(slisten, (LPSOCKADDR)&server_addr, sizeof(server_addr)) ==
        SOCKET_ERROR) {
        perror("could not bind.");
        exit(-1);
    }

    if (listen(slisten, 5) == SOCKET_ERROR) {
        perror("could not listen.");
        exit(-1);
    }

    SOCKET client_socket;
    sockaddr_in client_addr;
    int client_addr_size = sizeof(sockaddr_in);

    char buf[1024];

    vector<PROCESS_INFORMATION> pids;

    while (true) {
        client_socket =
            accept(slisten, (SOCKADDR*)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET) {
            perror("could not accept.");
            continue;
        }
        int time = 0;
        int ret = recv(client_socket, buf, sizeof(buf - 1), 0);
        if (ret > 0) {
            buf[ret] = '\0';
            time = atoi(buf);
        }

        LPTSTR cWinDir = new TCHAR[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, cWinDir);
        string tmpStr = string("subprocess.exe ") + to_string(time);
        LPTSTR sConLin = (char*)tmpStr.c_str();
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcess(NULL, sConLin, NULL, NULL, false, CREATE_NEW_CONSOLE,
                           NULL, NULL, &si, &pi)) {
            perror("could not create process.");
            exit(-1);
        }

        pids.emplace_back(pi);

        showRunning(pids);

        closesocket(client_socket);
    }

    closesocket(slisten);
    WSACleanup();
    return 0;
}

void showRunning(const vector<PROCESS_INFORMATION>& pids) {
    int cnt = 0;
    for (const auto& p : pids) {
        DWORD dw = WaitForSingleObject(p.hProcess, 5);

        if (dw == WAIT_TIMEOUT) {
            cnt++;
        } else if (dw == WAIT_FAILED) {
            cout << "error pid = " << p.dwProcessId << '\n';
        }
    }
    printf("current running clients = %d\n", cnt);
}

wchar_t* str2wchar(string str) {
    const char* pCStrkey = str.c_str();
    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrkey, strlen(pCStrkey) + 1,
                                    NULL, 0);
    wchar_t* pWCStrKey = new wchar_t[pSize];
    MultiByteToWideChar(CP_OEMCP, 0, pCStrkey, strlen(pCStrkey) + 1, pWCStrKey,
                        pSize);
    return pWCStrKey;
}