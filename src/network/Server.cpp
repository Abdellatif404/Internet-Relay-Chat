 
#include "Server.hpp"
#include "SocketHandler.hpp"

Server::Server(uint16_t port, const std::string& password)
	:	_serverFd(-1), _serverName("ircserv"),
		_serverVersion("1.0"), _port(port),
		_password(password), _eventLoop(NULL)
{
	_startTime = time(NULL);
}

Server::~Server()
{
	shutdown();
	delete _eventLoop;
	if (_serverFd >= 0)
		close(_serverFd);
}

void Server::start()
{
	_serverFd = SocketHandler::createSocket();

	SocketHandler::setSocketOptions(_serverFd);
	SocketHandler::setNonBlocking(_serverFd);
	SocketHandler::bindSocket(_serverFd, _port);
	SocketHandler::listenOnSocket(_serverFd);

	_eventLoop = new EventLoop(_serverFd, _password, _serverName, _serverVersion, _startTime);
	_eventLoop->run();
}

void Server::shutdown()
{
	// TODO: Broadcast shutdown message to all clients
	_eventLoop->stop();
}
