
#include "Connection.hpp"
#include "EventLoop.hpp"

Connection::Connection(int fd, strRef address, uint16_t port)
	: _fd(fd), _address(address), _port(port)
{
}

Connection::~Connection()
{
}

int Connection::getFd() const
{
	return _fd;
}

strRef Connection::getAddress() const
{
	return _address;
}

uint16_t Connection::getPort() const
{
	return _port;
}

void Connection::receiveData(MessageBuffer *messageBuffer)
{
	char	tempBuffer[1024];
	ssize_t recvBytes = recv(_fd, tempBuffer, sizeof(tempBuffer) - 1, 0);
	if (recvBytes > 0)
	{
		tempBuffer[recvBytes] = '\0';
		messageBuffer->storeMessage(_fd, tempBuffer, recvBytes);
	}
}

void Connection::sendData(SendQueue *sendQueue)
{
	std::string message = sendQueue->dequeueMessage(_fd);
	if (!message.empty())
		send(_fd, message.c_str(), message.size(), 0);
}
