#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

#define CLIENTSSIZE 10
#define BUFFERSIZE 1024

const string InitializeClientName = "100";
const string SendMess = "101";
const string EndConnection = "102";
const string NewClient = "103";


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

void RefreshListOfClientsToClients(Client& client);
void NotifyAllClientsAboutConnections(Client& client, bool isNewClient);
string CreateMessageToReviever(string& message, Client& sender, int clientId, string& badRes);
string ListAllClients(Client& currentClient);
DWORD WINAPI HandleRequest(LPVOID param);
void InitializeWSA();
SOCKET& CreateSocket();
void BindSocket(SOCKET& sock);
void Listen(SOCKET& sock);

int main()
{
    cout << "This Console Is Server" << endl << endl;

    SOCKET sock = SOCKET_ERROR;
   
    try
    {
        InitializeWSA();
        sock = CreateSocket();
        BindSocket(sock);
        Listen(sock);

        while (1) {

            SOCKET newClientSocket = accept(sock, NULL, NULL);

            if (newClientSocket == SOCKET_ERROR)
                throw new exception("Failed To Accept Request");

            cout << "Socket Accpeted a Client" << endl;

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
        }
        else if (action == SendMess) {

            int firstSeperatorIndex = input.find_first_of('|');
            input = input.substr(firstSeperatorIndex + 1);

            int firstSpaceIndex = input.find_first_of(' ');
            string senderMessage = input.substr(firstSpaceIndex + 1);

            int clientIdStringSize = (input.size() - senderMessage.size() - 1);
            int clientId = stoi(input.substr(0, clientIdStringSize));

            int sendToSocket;
            string result = "\nThis Client Is not Avaiable Now";

            if (clientId >= CLIENTSSIZE) {
                sendToSocket = client.sockID;
            }
            else {
                result = CreateMessageToReviever(senderMessage, client, clientId, result);

                Client reciever = clients[clientId];
                if (reciever.isConnectionClosed || strlen(reciever.name) == 0)
                    sendToSocket = client.sockID;
                else
                    sendToSocket = reciever.sockID;
            }        
            send(sendToSocket, result.data(), result.size(), 0);
        }
        else if (action == EndConnection) {
            NotifyAllClientsAboutConnections(client, false);
            client.isConnectionClosed = true;
            RefreshListOfClientsToClients(client);
            //closesocket(client.sockID);
        }
        else if (action == NewClient) {
            NotifyAllClientsAboutConnections(client, true);
            RefreshListOfClientsToClients(client);
        }
    }


    closesocket(client.sockID);
    return 0;
}

string ListAllClients(Client& currentClient) {
    string result = "\nAvailable Clients Are:\n";

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
        result = "\n\nThere are not Available Clients \n";

    return result;
}

string CreateMessageToReviever(string& message,Client& sender,int clientId,string& badRes) {
    Client reciever = clients[clientId];

    if (reciever.isConnectionClosed || strlen(reciever.name) == 0)
        return badRes;

    if (reciever.index == sender.index)
        return "\nYou Can Not Message Yourself";

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

void NotifyAllClientsAboutConnections(Client& client,bool isNewClient) {
    for (int i = 0; i < CLIENTSSIZE; i++)
    {
        Client reciever = clients[i];
        if (reciever.isConnectionClosed || reciever.index == client.index || strlen(reciever.name) == 0)
            continue;

        string clientName(client.name);
        string message = '\n' + clientName;

        if (isNewClient)
            message += " Connected To The System";
        else {
            message += " teminated his connection";
        }

        send(reciever.sockID, message.data(), message.size(), 0);
    }
}
void RefreshListOfClientsToClients(Client& client) {
    for (int i = 0; i < CLIENTSSIZE; i++)
    {
        Client reciever = clients[i];
        if (reciever.isConnectionClosed || strlen(reciever.name) == 0)
            continue;

        string message = ListAllClients(reciever); 
        send(reciever.sockID, message.data(), message.size(), 0);
    }
}