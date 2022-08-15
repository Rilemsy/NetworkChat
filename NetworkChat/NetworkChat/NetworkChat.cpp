
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include "SocketUtil.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include <ctime>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define GOOD_SEGMENT_SIZE 100
#define NAME_SIZE 20


int main(void)
{
	SocketUtil sockUtil;
	sockUtil.StaticInit();
    TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(INET);
    SocketAddress receivingAddres(INADDR_ANY, 48000);
    if (listenSocket->Bind(receivingAddres) != NO_ERROR)
    {
        return -1;
    }
    vector<TCPSocketPtr> readBlockSockets;
    readBlockSockets.push_back(listenSocket);
    vector<TCPSocketPtr> readableSockets;
    time_t workTime = time(0) + 170;
    
    while (time_t curTime = time(0) < workTime)
    {
        
        thread t([listenSocket] {
            listenSocket->Listen();
            });
        t.detach();
        if (SocketUtil::Select(&readBlockSockets, &readableSockets,
            nullptr, nullptr,
            nullptr, nullptr))
        {
            
            // получен пакет — обойти сокеты...
            for (const TCPSocketPtr& socket : readableSockets)
            {
                if (socket == listenSocket)
                {
                    // это сокет, принимающий запросы на соединение,
                    // принять новое соединение
                    SocketAddress newClientAddress;                
                        auto newSocket = listenSocket->Accept(newClientAddress);
                    readBlockSockets.push_back(newSocket);
                   // ProcessNewClient(newSocket, newClientAddress);
                    const char* pch = "Successful connection to server.\0";
                    newSocket->Send(pch, GOOD_SEGMENT_SIZE);
                    
                }
                else
                {
                    // это обычный сокет — обработать данные...
                    char segment[GOOD_SEGMENT_SIZE];
                    
                    int dataReceived =
                        socket->Receive(segment, GOOD_SEGMENT_SIZE);
                    //cout << "dataREcv : " << dataReceived << endl;
                    //cout << "segment : " << segment << endl;
                    if (dataReceived > 0)
                    {                    
                        char name[21];         
                        strncpy_s(name, segment, 20);
                        if (!socket->HaveName()) { // set client's namee
                            //cout << "setname\n";
                            socket->SetName(name);
                            //cout << "name length :" << strlen(name);
                            //cout << socket->GetName()<<endl;
                        }
                        else if (socket->HaveName() && readBlockSockets.size() > 2) {
                           // cout << "searchname\n";
                            for (const TCPSocketPtr& sock : readBlockSockets) // find receiver
                            {
                                //cout << "sock->GetName() : " << sock->GetName() << endl;
                                //cout << "name :" << name << endl;
                                
                                if (strncmp(sock->GetName(), name, strlen(sock->GetName())) == 0) {
                                    sock->Send(segment + NAME_SIZE, GOOD_SEGMENT_SIZE);
                                    //cout << "findname\n";
                                }

                            }
                            // ProcessDataFromClient(socket, segment, dataReceived);
                            //std::cout << "Message from Client : " << segment << endl;
                        }
                        
                        
                        
                    }
                    /*cout << "Message to Client: ";
                    char segmentTo[GOOD_SEGMENT_SIZE];
                    cin >> segmentTo;
                    listenSocket->Send(segmentTo, GOOD_SEGMENT_SIZE);
                    std::cout << "Message send.\n";*/
                }
            }
        }
        
    }
    sockUtil.CleanUp();
    std::cout << "Server shutdowned.\n";
    return 0;
}

