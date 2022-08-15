
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>
#include "D:\DevFiles\LearnC++\C++ for network\NetworkChat\NetworkChat\SocketUtil.h"
#include "D:\DevFiles\LearnC++\C++ for network\NetworkChat\NetworkChat\UDPSocket.h"
#include "D:\DevFiles\LearnC++\C++ for network\NetworkChat\NetworkChat\TCPSocket.h"
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

	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(INET);
	SocketAddress connAddres(2886732743, 48000); // 2130706433 = 127.0.0.1
	connSocket->Connect(connAddres);
	char segm[GOOD_SEGMENT_SIZE];
	int dataRecv =
		connSocket->Receive(segm, GOOD_SEGMENT_SIZE);
	if (dataRecv > 0)
	{
		cout << "Message from Server : " << segm << endl;
	}
	cout << "Enter your name (<20 symbols) : ";
	cin >> segm;
	char* pSegm;
	pSegm = segm + strlen(segm);
	*pSegm = '\0';
	connSocket->Send(segm, GOOD_SEGMENT_SIZE);
	time_t workTime = time(0) + 60;
	thread t([connSocket] {

		char segment[GOOD_SEGMENT_SIZE];
		time_t workTime = time(0) + 60;
		while (time_t curTime = time(0) < workTime) {
			int dataReceived =
				connSocket->Receive(segment, GOOD_SEGMENT_SIZE);
			//cout << "dataReceived: "<< dataReceived<<endl;
			if (dataReceived > 0)
			{
				// ProcessDataFromClient(socket, segment, dataReceived);
				cout << "      --- Message from Server : " << segment << endl;
			}
		}
		});
	t.detach();
	while (time_t curTime = time(0) < workTime)
	{
		
		//Sleep(1);
		char segmentTo[GOOD_SEGMENT_SIZE];
		cout << "Who receive a message : ";
		cin >> segmentTo;
		pSegm = segmentTo + strlen(segmentTo);
		*pSegm = '\0';
		cout << "Message to "<<segmentTo<<": ";
		*pSegm = '\n'; pSegm++;
		cin >> segmentTo+NAME_SIZE;
		connSocket->Send(segmentTo, GOOD_SEGMENT_SIZE);
		cout << "Message send.\n";
	}

	sockUtil.CleanUp();
	cout << "Client shutdowned.\n";
	return 0;
}