#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
using namespace std;

#define BUFFERSIZE 1024

const string InitializeClientName = "100";
const string SendMess = "101";
const string EndConnection = "102";
const string NewClient = "103";

bool isMessageValid(string input);
void InitializeWSA();
SOCKET& CreateSocket();
void Connect(SOCKET& sock);
void SendData(SOCKET& sock);
DWORD RecieveData(LPVOID sockPr);



int main()
{

    SOCKET sock = SOCKET_ERROR;

    try
    {
        cout << "This Console Is Client" << endl << endl;
        InitializeWSA();
        sock = CreateSocket();
        Connect(sock);

        DWORD threadId = 0;
        HANDLE t = CreateThread(NULL, 0, RecieveData, (LPVOID)&sock, 0, &threadId);

        SendData(sock);

        system("pause");
        closesocket(sock);
        WSACleanup();
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

void SendData(SOCKET& sock) {

    string input;

    cout << "Enter Your Name:";
    getline(std::cin, input);

    string message = InitializeClientName + '|' + input;
    send(sock, message.data(), message.size(), 0);

    cout << "\nIf You Want To End The Program Write: exit" << endl;
    cout << "Please, When You Write Your Message Write First The Number Of CLient You Want To Send The Message To Then Write Space Then Your Message" << endl;

    message = NewClient;
    send(sock, message.data(), message.size(), 0);
    
    while (1) {
        getline(std::cin, input);

        if (input == "exit") {
            input = EndConnection;
            send(sock, input.data(), input.size(), 0);
            return;
        }
        if (!isMessageValid(input)) {
            cout << "Please, When You Write Your Message Write First The Number Of CLient You Want To Send The Message To Then Write Space Then Your Message" << endl;
            cout << "Write Your Message:";
            continue;
        }

        string message = SendMess + '|' + input;
        send(sock, message.data(), message.size(), 0);

        Sleep(500);
        cout << "\nWrite Your Message:";
    }
}

DWORD RecieveData(LPVOID sockPr) {

    SOCKET sock = *((SOCKET*)sockPr);

    while (1) {

        char data[BUFFERSIZE];
        int read = recv(sock, data, BUFFERSIZE, 0);
        data[read] = '\0';
        string input(data);

        cout << input << endl;
        cout << "\nWrite Your Message:";
    }

    return 0;
}


bool isMessageValid(string input) {
    int firstSpaceIndex = input.find_first_of(' ');

    if (firstSpaceIndex == -1)
        return false;

    string clientStrId = input.substr(0, firstSpaceIndex);

    try
    {
        int clientId = stoi(clientStrId);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

