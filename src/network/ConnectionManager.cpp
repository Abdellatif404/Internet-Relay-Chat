 
#include "ConnectionManager.hpp"
#include "SocketHandler.hpp"

ConnectionManager::ConnectionManager(MessageBuffer *msgBuffer, SendQueue *sendQueue, 
									 eventVec &events, int epollFd)
	: _msgBuffer(msgBuffer), _sendQueue(sendQueue), 
	  _events(events), _epollFd(epollFd)
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

ConnectionMap &ConnectionManager::getConnections()
{
	return _connections;
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
		_msgBuffer->getBuffers().erase(fd);
		_sendQueue->getClientQueues().erase(fd);
		delete it->second;
		_connections.erase(it);
		_events[fd].data.fd = -1;
		_events[fd].events = 0;
		SocketHandler::removeSocket(_epollFd, fd);
		close(fd);
		std::cout << YELLOW << "Client #" << fd << " gone away" << RESET << std::endl;
	}
}
