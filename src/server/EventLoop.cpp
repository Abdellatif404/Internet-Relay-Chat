
#include "EventLoop.hpp"
#include "SocketHandler.hpp"
#include "EventHandler.hpp"
#include "BotManager.hpp"

extern bool g_running;

EventLoop::EventLoop(int serverFd, strRef pass, strRef srvName, strRef srvVersion, time_t startTime)
	: _srvFd(serverFd), _epFd(-1)
{
	(void)startTime;
	_epFd = EventHandler::initEpoll(_events);
	_msgBuffer = new MessageBuffer();
	_sendQueue = new SendQueue(_epFd);
	_connManager = new ConnectionManager(_msgBuffer, _sendQueue, _events, _epFd);
	_userManager = new UserManager(pass, _sendQueue, srvName, srvVersion);
	_chanManager = new ChannelManager(_sendQueue);
	_botManager = new BotManager(_userManager, _chanManager);
}

EventLoop::~EventLoop()
{
	delete _connManager;
	delete _userManager;
	delete _chanManager;
	delete _botManager;
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
				EventHandler::clientDisconnection(_connManager, _userManager, _chanManager, eventFd);
				continue;
			}
			if (eventFlags & EPOLLIN)
			{
				Connection		*conn = _connManager->getConnection(eventFd);
				if (!conn)
					continue;
				EventHandler::recvFromClient(_connManager, _chanManager, _userManager, _botManager, conn, _msgBuffer, _sendQueue);
			}
			if (eventFlags & EPOLLOUT)
			{
				Connection		*conn = _connManager->getConnection(eventFd);
				if (!conn)
					continue;
				EventHandler::sendToClient(conn, _sendQueue, _epFd);
			}
		}
	}
}

void EventLoop::run()
{
	SocketHandler::addSocket(_epFd, _srvFd);
	std::cout << GREEN << "Server is running..." << RESET << std::endl;
	g_running = true;
	while (g_running)
		handleEvents();
}

void EventLoop::stop()
{
	std::string message = "Server is shutting down...\r\n";
	ConnectionMap _connections = _connManager->getConnections();
	for (ConnectionMap::iterator it = _connections.begin(); it != _connections.end(); it++)
	{
		_sendQueue->enqueueMessage(it->second->getFd(), message);
		it->second->sendData(_sendQueue);
	}
}
