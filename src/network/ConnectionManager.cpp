
#include "ConnectionManager.hpp"
#include "SocketHandler.hpp"

ConnectionManager::ConnectionManager(const std::string& password)
{
	_userManager = new UserManager(password);
}

ConnectionManager::~ConnectionManager()
{
}

int	ConnectionManager::createConnection(int socketFd)
{
	struct sockaddr_in	clientAddr;
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	int	clientFd = SocketHandler::acceptConnection(socketFd, (soaddr_t *)&clientAddr);
	SocketHandler::setNonBlocking(clientFd);

	std::string	ip = inet_ntoa(clientAddr.sin_addr);
	uint16_t	port = ntohs(clientAddr.sin_port);

	User		*newUser = _userManager->createUser(clientFd);
	Connection	*conn = new Connection(clientFd, ip, port);
	conn->setUser(newUser);
	_connections[clientFd] = conn;
	return conn->getFd();
}

Connection *ConnectionManager::getConnection(int fd)
{
	std::map<int, Connection*>::iterator it = _connections.find(fd);
	if (it != _connections.end())
		return it->second;
	return NULL;
}
