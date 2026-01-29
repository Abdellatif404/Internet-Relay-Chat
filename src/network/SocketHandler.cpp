
#include "SocketHandler.hpp"

void SocketHandler::_protect(int status, strRef errorMsg)
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
	if (clientFd < 0)
	{
		std::cerr << YELLOW << "Failed to accept new connection: " << strerror(errno) << RESET << std::endl;
		return -1;
	}
	std::cout << GREEN << "New client #" << clientFd << " from "
			  << inet_ntoa(((struct sockaddr_in *)addr)->sin_addr) << ":"
			  << ntohs(((struct sockaddr_in *)addr)->sin_port) << RESET << std::endl;
	return clientFd;
}

void SocketHandler::addSocket(int epFd, int fd)
{
	event_t	event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	event.data.fd = fd;

	int exitCode = epoll_ctl(epFd, EPOLL_CTL_ADD, fd, &event);
	_protect(exitCode, "Failed to add socket to epoll");
}

void SocketHandler::modifySocket(int epFd, int fd, uint32_t events)
{
	event_t	event;
	std::memset(&event, 0, sizeof(event));
	event.events = events | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	event.data.fd = fd;

	int exitCode = epoll_ctl(epFd, EPOLL_CTL_MOD, fd, &event);
	if (exitCode < 0)
		std::cerr << YELLOW << "Failed to modify socket in epoll: " << strerror(errno) << RESET << std::endl;
}

void SocketHandler::removeSocket(int epFd, int fd)
{
	int exitCode = epoll_ctl(epFd, EPOLL_CTL_DEL, fd, NULL);
	_protect(exitCode, "Failed to remove socket from epoll");
}
