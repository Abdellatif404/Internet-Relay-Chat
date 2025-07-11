
#include "EventLoop.hpp"
#include "SocketHandler.hpp"

EventLoop::EventLoop(int serverFd, const std::string& password)
	: _serverFd(serverFd), _epollFd(-1)
{
	_epollFd = epoll_create1(0);
	_protect(_epollFd, "Failed to create epoll instance");

	_connectionManager = new ConnectionManager(password);
	_channelManager = new ChannelManager();

	_events.resize(1024);
	for (size_t i = 0; i < _events.size(); i++)
		std::memset(&_events[i], 0, sizeof(struct epoll_event));
}

EventLoop::~EventLoop()
{
	if (_epollFd >= 0)
		close(_epollFd);
	delete _connectionManager;
	delete _channelManager;
}

void EventLoop::_protect(int status, const std::string& errorMsg)
{
	if (status < 0)
	{
		if (_epollFd >= 0)
		{
			close(_epollFd);
			_epollFd = -1;
		}
		throw std::runtime_error(errorMsg);
	}
}

void EventLoop::addSocket(int fd)
{
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event);
	_protect(exitCode, "Failed to add socket to epoll");
}

void EventLoop::modifySocket(int fd, uint32_t events)
{
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = events;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event);
	_protect(exitCode, "Failed to modify socket in epoll");
}

void EventLoop::removeSocket(int fd)
{
	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
	_protect(exitCode, "Failed to remove socket from epoll");
}

void EventLoop::handleEvents()
{
	int		eventCount = epoll_wait(_epollFd, _events.data(), _events.size(), -1);
	_protect(eventCount, "Failed to wait for epoll events");

	for (int i = 0; i < eventCount; i++)
	{
		if (_events[i].data.fd == _serverFd)
		{
			if (_events[i].events & EPOLLIN)
			{
				std::cout << YELLOW << "New connection detected on server socket" << RESET << std::endl;

				int connFd = _connectionManager->createConnection(_serverFd);
				addSocket(connFd);
			}
		}
		else
		{
			if (_events[i].events & EPOLLIN)
			{
				Connection *conn = _connectionManager->getConnection(_events[i].data.fd);
				if (conn)
					conn->receiveData();
			}
			if (_events[i].events & (EPOLLERR | EPOLLRDHUP))
			{
				std::cout << RED << "Client " << _events[i].data.fd << " disconnected" << RESET << std::endl;
				removeSocket(_events[i].data.fd);
				close(_events[i].data.fd);
			}
		}
	}
}

void EventLoop::run()
{
	addSocket(_serverFd);
	std::cout << GREEN << "Server is running..." << RESET << std::endl;
	while (true)
	{
		handleEvents();
	}
}
