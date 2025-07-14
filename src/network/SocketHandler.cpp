
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

void SocketHandler::setSocketOptions(int fd)
{
	int opt = 1;
	int exitCode = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	_protect(exitCode, "Failed to set socket options");
}

void SocketHandler::setNonBlocking(int fd)
{
	int exitCode = fcntl(fd, F_SETFL, O_NONBLOCK);
	_protect(exitCode, "Failed to set socket to non-blocking mode");
}

void SocketHandler::bindSocket(int fd, uint16_t port)
{
	struct sockaddr_in	serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	int exitCode = bind(fd, (soaddr_t *)&serverAddr, sizeof(serverAddr));
	_protect(exitCode, "Failed to bind socket");
}

void SocketHandler::listenOnSocket(int fd)
{
	int exitCode = listen(fd, SOMAXCONN);
	_protect(exitCode, "Failed to listen on socket");
}

int SocketHandler::acceptConnection(int fd, soaddr_t *addr)
{
	socklen_t clientAddrLen = sizeof(addr);
	int clientFd = accept(fd, addr, &clientAddrLen);
	_protect(clientFd, "Failed to accept new connection");
	return clientFd;
}
