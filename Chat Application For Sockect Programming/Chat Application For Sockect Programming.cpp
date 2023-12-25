//#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "iostream"

using namespace std;

int main()
{
    WSADATA wsaData;
    WORD wsaVersion = MAKEWORD(2, 2);
    int wsaError = WSAStartup(wsaVersion, &wsaData);

    if (wsaError != 0) {
        cout << "Failed to Startup !\n";
        return 0;
    }

    SOCKET sockett = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockett == INVALID_SOCKET) {
        cout << "Failed To Create a Socket !\n";
        WSACleanup();
        return 0;
    }


    cout << "Startup Succeeded !\n";
    cout << wsaData.szSystemStatus;
    

    closesocket(sockett);
    WSACleanup();
    return 0;
}