#pragma once
#include "SocketAddress.h"

using namespace std;

class TCPSocket
{
public:
	~TCPSocket();
	void							SetName(char* name);
	char*							GetName();
	bool							HaveName();
	int								Connect(const SocketAddress& inAddress);
	int								Bind(const SocketAddress& inToAddress);
	int								Listen(int inBackLog = 32);
	shared_ptr< TCPSocket >			Accept(SocketAddress& inFromAddress);
	int32_t							Send(const void* inData, size_t inLen);
	int32_t							Receive(void* inBuffer, size_t inLen);
private:
	friend class SocketUtil;
	TCPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET		mSocket;
	char clientName[20];
	bool haveName = FALSE;
};
typedef shared_ptr< TCPSocket > TCPSocketPtr;