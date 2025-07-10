
#include "SocketHandler.hpp"

void SocketHandler::_protect(int status, const std::string& errorMsg)
{
	if (status < 0)
		throw std::runtime_error(errorMsg);
}

int SocketHandler::createSocket()
{
	int socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_protect(socketFd, "Failed to create socket");
	return socketFd;
}

void SocketHandler::setSocketOptions(int socketFd)
{
	int opt = 1;
	int exitCode = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	_protect(exitCode, "Failed to set socket options");
}

void SocketHandler::setNonBlocking(int socketFd)
{
	int exitCode = fcntl(socketFd, F_SETFL, O_NONBLOCK);
	_protect(exitCode, "Failed to set socket to non-blocking mode");
}

void SocketHandler::bindSocket(int socketFd, uint16_t port)
{
	struct sockaddr_in	serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	int exitCode = bind(socketFd, (soaddr_t *)&serverAddr, sizeof(serverAddr));
	_protect(exitCode, "Failed to bind socket");
}

void SocketHandler::listenOnSocket(int socketFd)
{
	int exitCode = listen(socketFd, SOMAXCONN);
	_protect(exitCode, "Failed to listen on socket");
}

int SocketHandler::acceptConnection(int socketFd, soaddr_t *addr)
{
	socklen_t clientAddrLen = sizeof(addr);
	int clientFd = accept(socketFd, addr, &clientAddrLen);
	_protect(clientFd, "Failed to accept new connection");
	return clientFd;
}
