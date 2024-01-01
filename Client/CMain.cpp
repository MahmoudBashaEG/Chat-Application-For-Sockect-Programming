#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
using namespace std;


#define BUFFERSIZE 1024

const string InitializeClientName = "100";
const string ListCLients = "101";
const string SendMess = "102";
const string EndConnection = "103";

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
        InitializeWSA();
        sock = CreateSocket();

        //Sleep(4000);
        Connect(sock);

        DWORD threadId = 0;
        HANDLE t = CreateThread(NULL, 0, RecieveData, (LPVOID)&sock, 0, &threadId);

        SendData(sock);

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

    cout << "If You Want To End The Program Write: exit" << endl;
    cout << "If You Want To Refresh The Client List Write: list" << endl;
    cout << "Please, When You Write Your Message Write First The Number Of CLient You Want To Send The Message To Then Write | Then Your Message" << endl;

    Sleep(1000);


    message = ListCLients;
    send(sock, message.data(), message.size(), 0);


    while (1) {

        cout << "Write Your Message:";

        cin >> input;
        if (input == "exit") {
            input = EndConnection;
            send(sock, input.data(), input.size(), 0);
            return;
        }
        else if (input == "list") {
            input = ListCLients;
            send(sock, input.data(), input.size(), 0);

            Sleep(1000);
            continue;
        }

        if (!isMessageValid(input)) {
            cout << "Please, When You Write Your Message Write First The Number Of CLient You Want To Send The Message To Then Write | Then Your Message" << endl << endl;
            continue;
        }

        input = SendMess + '|' + input;
        send(sock, input.data(), input.size(), 0);
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

    }

    return 0;
}


bool isMessageValid(string input) {
    int firstSpaceIndex = input.find('|');

    if (firstSpaceIndex == -1)
        return false;

    string clientStrId = input.substr(0, firstSpaceIndex);
    string message = input.substr(0, firstSpaceIndex);

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

