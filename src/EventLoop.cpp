
#include "EventLoop.hpp"

EventLoop::EventLoop(int serverFd)
	: _serverFd(serverFd)
{
	#ifdef __linux__
	_epollFd = epoll_create1(0);
	_protect(_epollFd, "Failed to create epoll instance");

	_events.resize(10);
	_readBuffer.resize(1024);
	#endif
}

EventLoop::~EventLoop()
{
	if (_epollFd >= 0)
		close(_epollFd);
}

void EventLoop::_protect(int status, const std::string& errorMsg)
{
	if (status < 0)
	{
		if (_epollFd >= 0)
			close(_epollFd);
		throw std::runtime_error(errorMsg);
	}
}

void EventLoop::run()
{
	addSocket(_serverFd);
	while (true)
	{
		handleEvents();
	}
}

void EventLoop::addSocket(int fd)
{
	#ifdef __linux__
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event);
	_protect(exitCode, "Failed to add socket to epoll");
	#endif
}

void EventLoop::modifySocket(int fd, int events)
{
	#ifdef __linux__
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = events;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &event);
	_protect(exitCode, "Failed to modify socket in epoll");
	#endif
}

void EventLoop::removeSocket(int fd)
{
	#ifdef __linux__
	int exitCode = epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
	_protect(exitCode, "Failed to remove socket from epoll");
	#endif
}

void EventLoop::handleEvents()
{
	#ifdef __linux__
	int clientFd;
	int bytesRead;
	int numEvents = epoll_wait(_epollFd, _events.data(), _events.size(), -1);
	_protect(numEvents, "Failed to wait for epoll events");

	for (int i = 0; i < numEvents; i++)
	{
		if (_events[i].data.fd == _serverFd)
		{
			clientFd = accept(_serverFd, NULL, NULL);
			_protect(clientFd, "Failed to accept new connection");

			int exitCode = fcntl(clientFd, F_SETFL, O_NONBLOCK);
			_protect(exitCode, "Failed to set client socket to non-blocking mode");

			addSocket(clientFd);
			std::cout << "New client connected: " << clientFd << std::endl;
		}
		else
		{
			bytesRead = recv(_events[i].data.fd, _readBuffer.data(), _readBuffer.size(), 0);
			if (bytesRead <= 0)
			{
				close(_events[i].data.fd);
				removeSocket(_events[i].data.fd);
			}
			else
			{
				_readBuffer[bytesRead] = '\0';
				std::cout << "Client " << _events[i].data.fd << " sent: " << _readBuffer.data();
			}
		}
	}
	#endif
}
