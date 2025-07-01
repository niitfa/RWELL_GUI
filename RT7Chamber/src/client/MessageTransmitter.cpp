#include "MessageTransmitter.h"

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <fcntl.h>
#endif

#include <thread>
#include <iostream>

MessageTransmitter::MessageTransmitter()
{
	memset(this->message, 0, kMessageSize);
}

int MessageTransmitter::Connect(std::string ip, uint16_t port)
{
    this->Disconnect();

    /*WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		return 1;
	}

	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(this->ip.c_str());
	addr.sin_port = htons(this->port);
    addr.sin_family = AF_INET; */



	this->sock = socket(AF_INET, SOCK_STREAM, NULL);

	if (this->sock == -1)
	{
		return 2;
	}

	//if (connect(this->sock, (SOCKADDR*)&addr, addrLen) < 0)
	//{
	//	return 3;
	//}
	while (connect(this->sock, (SOCKADDR*)&addr, addrLen) < 0)
		;

	return 0;
}

int MessageTransmitter::Disconnect()
{
	closesocket(this->sock);
	return 0;
}


int MessageTransmitter::Send(int val_1, int val_2)
{
	this->mtx.lock();
	int send_size = 0;
	memset(this->message, 0, this->kMessageSize);
	memcpy(this->message + this->kBytePositionValue1, &val_1, sizeof(val_1));
	memcpy(this->message + this->kBytePositionValue2, &val_2, sizeof(val_2));
	send_size = send(this->sock, this->message, this->kMessageSize, 0);
	this->mtx.unlock();
	return send_size;
}
