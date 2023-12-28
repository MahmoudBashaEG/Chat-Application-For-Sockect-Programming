#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
using namespace std;


void InitializeWSA();
SOCKET& CreateSocket();
void BindSocket(SOCKET& sock);
void Listen(SOCKET& sock);

int main()
{

    SOCKET sock = SOCKET_ERROR;
   
    try
    {
        InitializeWSA();
        sock = CreateSocket();
        BindSocket(sock);
        Listen(sock);



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

void BindSocket(SOCKET& sock) {

    sockaddr_in bindingData;
    bindingData.sin_family = AF_INET;
    bindingData.sin_port = htons(55555);

    inet_pton(AF_INET, "127.0.0.1", &bindingData.sin_addr);

    int bindResult = bind(sock, (SOCKADDR*)&bindingData, sizeof(bindingData));
    if (bindResult == SOCKET_ERROR)
        throw new exception("Failed To Bind a Socket !\n");

    cout << "Socket is binded" << endl;
}

void Listen(SOCKET& sock) {

    sockaddr_in bindingData;
    bindingData.sin_family = AF_INET;
    bindingData.sin_port = htons(55555);

    inet_pton(AF_INET, "127.0.0.1", &bindingData.sin_addr);

    int listedResult = listen (sock, 4);
    if (listedResult == SOCKET_ERROR)
        throw new exception("Failed To Listen a Socket !\n");

    cout << "Socket is listening" << endl;
}

