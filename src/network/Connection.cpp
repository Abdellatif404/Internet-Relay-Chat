
#include "Connection.hpp"
#include "EventLoop.hpp"

Connection::Connection(int fd, const std::string& address, uint16_t port)
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

const std::string& Connection::getAddress() const
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
	{
		ssize_t sentBytes = send(_fd, message.c_str(), message.size(), 0);
		if (sentBytes < static_cast<ssize_t>(message.size()))
		{
			sendQueue->enqueueMessage(_fd, message.substr(sentBytes));
		}
	}
}
