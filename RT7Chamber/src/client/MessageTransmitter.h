#ifndef STEPPER_CONTROLLER_TRANSMITTER_H
#define STEPPER_CONTROLLER_TRANSMITTER_H

#include <stdint.h>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

class MessageTransmitter
{
	static const int kMessageSize = 8;

	static const int kBytePositionValue1 = 0;
	static const int kBytePositionValue2 = 4;

	std::string ip;
    uint16_t port;

    int sock;
	char message[kMessageSize];

	std::mutex mtx;
public:
    MessageTransmitter();

	~MessageTransmitter() = default;

    int Connect(std::string ip, uint16_t port);
	int Disconnect();

	int Send(
		int val_1,
		int val_2
	);
};

#endif

