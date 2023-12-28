#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;

void InitializeWSA();
SOCKET& CreateSocket();
void Connect(SOCKET& sock);



int main()
{

    SOCKET sock = SOCKET_ERROR;

    try
    {
        InitializeWSA();
        sock = CreateSocket();
        Connect(sock);

        return 0;
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
        cout << WSAGetLastError() << endl;

        if (sock != SOCKET_ERROR)
            closesocket(sock);

        WSACleanup();

        return -1;
    }
}


void InitializeWSA() {
    WSADATA wsaData;
    WORD wsaVersion = MAKEWORD(2, 2);
    int wsaError = WSAStartup(wsaVersion, &wsaData);

    if (wsaError != 0)
        throw new exception("Failed to Initialize WSA !\n");

    cout << "WSA is initialized" << endl;

}

SOCKET& CreateSocket() {


    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET)
        throw new exception("Failed To Create a Socket !\n");

    cout << "Socket is created" << endl;
    return sock;
}

void Connect(SOCKET& sock) {

    sockaddr_in bindingData;
    bindingData.sin_family = AF_INET;
    bindingData.sin_port = htons(55555);

    inet_pton(AF_INET, "127.0.0.1", &bindingData.sin_addr);

    int connectResult = connect(sock, (SOCKADDR*)&bindingData, sizeof(bindingData));
    if (connectResult == SOCKET_ERROR)
        throw new exception("Failed To Connect a Socket !\n");

    cout << "Socket is connected" << endl;
}

