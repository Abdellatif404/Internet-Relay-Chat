 
#include "Server.hpp"
#include "SocketHandler.hpp"

Server::Server(uint16_t port, const std::string& password)
	: _serverFd(-1), _port(port), _password(password), _eventLoop(NULL)
{
}

Server::~Server()
{
	if (_serverFd >= 0)
	{
		close(_serverFd);
		_serverFd = -1;
	}
	delete _eventLoop;
}

void Server::_protect(int status, const std::string& errorMsg)
{
	if (status < 0)
	{
		if (_serverFd >= 0)
			close(_serverFd);
		throw std::runtime_error(errorMsg);
	}
}

void Server::start()
{
	_serverFd = SocketHandler::createSocket();

	SocketHandler::setSocketOptions(_serverFd);
	SocketHandler::setNonBlocking(_serverFd);
	SocketHandler::bindSocket(_serverFd, _port);
	SocketHandler::listenOnSocket(_serverFd);

	_eventLoop = new EventLoop(_serverFd, _password);
	_eventLoop->run();
}
