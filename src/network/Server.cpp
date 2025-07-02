
#include "Server.hpp"

Server::Server(int port, const std::string& password)
	: _port(port), _password(password)
{
}

Server::~Server()
{
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
	_serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_protect(_serverFd, "Failed to create socket");

	int exitCode;
	int	opt = 1;
	exitCode = setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	_protect(exitCode, "Failed to set socket options");

	#ifdef __linux__
	exitCode = fcntl(_serverFd, F_SETFL, O_NONBLOCK);
	_protect(exitCode, "Failed to set socket to non-blocking mode");
	#endif

	struct sockaddr_in	serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(_port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	exitCode = bind(_serverFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	_protect(exitCode, "Failed to bind socket");

	exitCode = listen(_serverFd, SOMAXCONN);
	_protect(exitCode, "Failed to listen on socket");
	
	EventLoop eventLoop(_serverFd);

	std::cout << GREEN << "Server started on port " << _port << RESET << std::endl;
	eventLoop.run();
}
