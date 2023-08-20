
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
            
            // ïîëó÷åí ïàêåò — îáîéòè ñîêåòû...
            for (const TCPSocketPtr& socket : readableSockets)
            {
                if (socket == listenSocket)
                {
                    // ýòî ñîêåò, ïðèíèìàþùèé çàïðîñû íà ñîåäèíåíèå,
                    // ïðèíÿòü íîâîå ñîåäèíåíèå
                    SocketAddress newClientAddress;                
                        auto newSocket = listenSocket->Accept(newClientAddress);
                    readBlockSockets.push_back(newSocket);           
                    const char* pch = "Successful connection to server.\0";
                    newSocket->Send(pch, GOOD_SEGMENT_SIZE);
                    
                }
                else
                {
                    // ýòî îáû÷íûé ñîêåò — îáðàáîòàòü äàííûå...
                    char segment[GOOD_SEGMENT_SIZE];
                    
                    int dataReceived =
                        socket->Receive(segment, GOOD_SEGMENT_SIZE);
                    if (dataReceived > 0)
                    {                    
                        char name[21];         
                        strncpy_s(name, segment, 20);
                        if (!socket->HaveName()) { // set client's namee
                            socket->SetName(name);
                        }
                        else if (socket->HaveName() && readBlockSockets.size() > 2) {
                            for (const TCPSocketPtr& sock : readBlockSockets) // find receiver
                            {
                                if (strncmp(sock->GetName(), name, strlen(sock->GetName())) == 0) {
                                    sock->Send(segment + NAME_SIZE, GOOD_SEGMENT_SIZE);
                                }
                            }
                        }  
                    }
                }
            }
        }
        
    }
    sockUtil.CleanUp();
    std::cout << "Server shutdowned.\n";
    return 0;
}

