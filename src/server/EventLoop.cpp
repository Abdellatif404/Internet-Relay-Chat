
#include "EventLoop.hpp"
#include "SocketHandler.hpp"
#include "EventHandler.hpp"

EventLoop::EventLoop(int serverFd, strRef pass, strRef srvName, strRef srvVersion, time_t startTime)
	: _srvFd(serverFd), _epFd(-1)
{
	(void)srvVersion;
	(void)startTime;
	
	_epFd = EventHandler::initEpoll(_events);

	_msgBuffer = new MessageBuffer();
	_sendQueue = new SendQueue(_epFd);
	_connManager = new ConnectionManager();
	_userManager = new UserManager(pass, _sendQueue, srvName);
	_chanManager = new ChannelManager(_sendQueue);
}

EventLoop::~EventLoop()
{
	delete _connManager;
	delete _userManager;
	delete _chanManager;
	delete _msgBuffer;
	delete _sendQueue;
	if (_epFd >= 0)
		close(_epFd);
}

void EventLoop::handleEvents()
{
	int	eventCount = EventHandler::waitForEvents(_epFd, _events);

	for (int i = 0; i < eventCount; i++)
	{
		int			eventFd = _events[i].data.fd;
		uint32_t	eventFlags = _events[i].events;

		if (eventFd == _srvFd)
		{
			if (eventFlags & EPOLLIN)
				EventHandler::newConnection(_connManager, _srvFd, _epFd);
		}
		else
		{
			if (eventFd < 0)
				continue;
			if (eventFlags & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				EventHandler::clientDisconnection(_connManager, _userManager, _epFd, eventFd, _events[i]);
				continue;
			}
			if (eventFlags & (EPOLLIN | EPOLLOUT))
			{
				Connection		*conn = _connManager->getConnection(eventFd);
				if (!conn)
					continue;
				if (eventFlags & EPOLLIN)
					EventHandler::recvFromClient(_chanManager, _userManager, conn, _msgBuffer, _sendQueue);
				if (eventFlags & EPOLLOUT)
					EventHandler::sendToClient(conn, _sendQueue, _epFd);
			}
		}
	}
}

void EventLoop::run()
{
	SocketHandler::addSocket(_epFd, _srvFd);
	std::cout << GREEN << "Server is running..." << RESET << std::endl;
	while (true)
		handleEvents();
}

void EventLoop::stop()
{
	std::string message = "Warning: Server is shutting down...\r\n";
	ConnectionMap _connections = _connManager->getConnections();
	for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); it++)
	{
		_sendQueue->enqueueMessage(it->second->getFd(), message);
		it->second->sendData(_sendQueue);
	}
}
