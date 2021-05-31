#include <Windows.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    int time = atoi(argv[1]);
    printf("new client pid = %d, server_time = %s\n", GetCurrentProcessId(),
           argv[1]);
    Sleep(time * 1000);
    ExitProcess(1001);
    return 0;
}