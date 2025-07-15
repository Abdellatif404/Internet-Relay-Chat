 
#include "ConnectionManager.hpp"
#include "SocketHandler.hpp"

ConnectionManager::ConnectionManager()
{
}

ConnectionManager::~ConnectionManager()
{
	for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); it++)
	{
		delete it->second;
	}
}

Connection *ConnectionManager::getConnection(int fd)
{
	ConnectionMap::iterator it = _connections.find(fd);
	if (it != _connections.end())
		return it->second;
	return NULL;
}

int	ConnectionManager::createConnection(int fd)
{
	struct sockaddr_in	clientAddr;
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	int	clientFd = SocketHandler::acceptConnection(fd, (soaddr_t *)&clientAddr);
	if (clientFd < 0)
		return -1;
	SocketHandler::setNonBlocking(clientFd);

	std::string	ip = inet_ntoa(clientAddr.sin_addr);
	uint16_t	port = ntohs(clientAddr.sin_port);

	Connection	*conn = new Connection(clientFd, ip, port);
	_connections[clientFd] = conn;
	return conn->getFd();
}

void ConnectionManager::removeConnection(int fd)
{
	ConnectionMap::iterator it = _connections.find(fd);
	if (it != _connections.end())
	{
		Connection *conn = it->second;
		delete conn;
		_connections.erase(it);
	}
}
