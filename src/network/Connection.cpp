
#include "Connection.hpp"

Connection::Connection(int fd, const std::string& address, uint16_t port)
	: _fd(fd), _address(address), _port(port), _user(NULL)
{
	_recvBuffer.resize(1024);
	std::memset(_recvBuffer.data(), 0, _recvBuffer.size());
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

User *Connection::getUser() const
{
	return _user;
}

void Connection::setUser(User *user)
{
	_user = user;
}

void Connection::receiveData()
{
	ssize_t recvBytes = recv(_fd, _recvBuffer.data(), _recvBuffer.size() - 1, 0);
	if (recvBytes > 0)
	{
		_recvBuffer[recvBytes] = '\0';
	}
}
