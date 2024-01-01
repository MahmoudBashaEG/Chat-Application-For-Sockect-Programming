#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

#define CLIENTSSIZE 10
#define BUFFERSIZE 1024


const string InitializeClientName = "100";
const string ListCLients = "101";
const string SendMess = "102";
const string EndConnection = "103";

struct Client {

    int index;
    long sockID;
    struct sockaddr_in clientAddr;
    int len;
    char name[20] = "";
    bool isConnectionClosed = false;
};

int clientCount = 0;

struct Client clients[CLIENTSSIZE];
HANDLE threads[CLIENTSSIZE];

string CreateMessageToReviever(string& message, Client& sender, Client& reciever);
string ListAllClients(Client& currentClient);
DWORD WINAPI HandleRequest(LPVOID param);
void InitializeWSA();
SOCKET& CreateSocket();
void BindSocket(SOCKET& sock);
void Listen(SOCKET& sock);

int main()
{
    cout << "This Consile Is Server" << endl << endl;

    SOCKET sock = SOCKET_ERROR;
   
    try
    {
        InitializeWSA();
        sock = CreateSocket();
        BindSocket(sock);
        Listen(sock);

        while (1) {

            SOCKET newClientSocket = accept(sock, NULL, NULL); //(sockaddr*)&clients[clientCount].clientAddr, &clients[clientCount].len);

            if (newClientSocket == SOCKET_ERROR)
                throw new exception("Failed To Accept Request");

            cout << "Socket Accpeted One Client" << endl;

            clients[clientCount].sockID = newClientSocket;
            clients[clientCount].index = clientCount;

            DWORD threadId = 0;
            threads[clientCount] = CreateThread(NULL, 0, HandleRequest, (LPVOID)&clients[clientCount], 0, &threadId);
            clientCount++;
        }


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


DWORD WINAPI HandleRequest(LPVOID param)
{
    Client clientData = *((Client*)param);

    Client& client = clients[clientData.index];
    char clientBuffer[BUFFERSIZE];

    while (1) {

        int readBytesCount = recv(client.sockID, clientBuffer, BUFFERSIZE, 0);
        clientBuffer[readBytesCount] = '\0';
        string input(clientBuffer);

        string action = input.substr(0, 3);

        if (action == InitializeClientName) {
            string data = input.substr(4);
            strcpy_s(client.name, data.data());

            cout << client.name << endl;
        }
        else if (action == ListCLients) {
            string res = ListAllClients(client);
            send(client.sockID, res.data(), res.size(), 0);
        }
        else if (action == SendMess) {

            int firstSeperatorIndex = input.find_first_of('|');
            input = input.substr(firstSeperatorIndex + 1);

            int lastSeperatorIndex = input.find_first_of(' ');

            string senderMessage = input.substr(lastSeperatorIndex + 1);

            int clientIdStringSize = (input.size() - senderMessage.size() - 1);
            int clientId = stoi(input.substr(0, clientIdStringSize));

            Client reciever = clients[clientId];
            string recieverMessage = CreateMessageToReviever(senderMessage, client, reciever);

            int sendToSocket;
            if (reciever.isConnectionClosed || strlen(reciever.name) == 0)
                sendToSocket = client.sockID;
            else
                sendToSocket = reciever.sockID;

            send(sendToSocket, recieverMessage.data(), recieverMessage.size(), 0);
        }
        else if (action == EndConnection) {
            client.isConnectionClosed = true;

            for (int i = 0; i < CLIENTSSIZE; i++)
            {
                Client reciever = clients[i];
                if (reciever.isConnectionClosed || reciever.index == client.index)
                    continue;

                string clientName(client.name);
                string message = '\n' + clientName + " teminated his connection";
                send(reciever.sockID, message.data(), message.size(), 0);
            }

            closesocket(client.sockID);
        }
    }


    closesocket(client.sockID);
    return 0;
}

string ListAllClients(Client& currentClient) {
    string result = "\n\nClients Are: \n";

    int notReadyClients = 0;

    for (int i = 0; i < CLIENTSSIZE; i++) {
        Client client = clients[i];

        if (client.index == currentClient.index || client.isConnectionClosed || strlen(client.name) == 0) {
            notReadyClients++;
            continue;
        }

        result += (to_string(client.index) + ":" + client.name) + "\n";
    }
    
    if (notReadyClients == CLIENTSSIZE)
        result = "\n\n There isn't Available Clients \n";

    return result;
}

string CreateMessageToReviever(string& message,Client& sender, Client& reciever) {
 
    if (reciever.isConnectionClosed || strlen(reciever.name) == 0)
        return '\n' + "This Client Isn't Avaiable Now";

    string nameOfSender(sender.name);

    return '\n' + nameOfSender + ": " + message;
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
    int listedResult = listen (sock, CLIENTSSIZE);
    if (listedResult == SOCKET_ERROR)
        throw new exception("Failed To Listen a Socket !\n");

    cout << "Socket is listening" << endl;
}

