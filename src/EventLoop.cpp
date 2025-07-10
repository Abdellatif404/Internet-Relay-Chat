
#include "EventLoop.hpp"

EventLoop::EventLoop(int serverFd)
	: _serverFd(serverFd)
{
	_epollFd = epoll_create1(0);
	_protect(_epollFd, "Failed to create epoll instance");

	_events.resize(1024);
	std::memset(_events.data(), 0, _events.size() * sizeof(struct epoll_event));
	_readBuffer.resize(1024);
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
	int		clientFd;
	ssize_t recvBytes;
	int		eventCount = epoll_wait(_epollFd, _events.data(), _events.size(), -1);
	_protect(eventCount, "Failed to wait for epoll events");

	for (int i = 0; i < eventCount; i++)
	{
		if (_events[i].data.fd == _serverFd)
		{
			if (_events[i].events & EPOLLIN)
			{
				std::cout << YELLOW << "New connection detected on server socket" << RESET << std::endl;

				struct sockaddr_in clientAddr;
				std::memset(&clientAddr, 0, sizeof(clientAddr));

				clientFd = SocketHandler::acceptConnection(_serverFd, (soaddr_t *)&clientAddr);
				SocketHandler::setNonBlocking(clientFd);
				addSocket(clientFd);
			}
		}
		else
		{
			if (_events[i].events & EPOLLIN)
			{
				recvBytes = recv(_events[i].data.fd, _readBuffer.data(), _readBuffer.size() - 1, 0);
				if (recvBytes > 0)
				{
					_readBuffer[recvBytes] = '\0';
					std::cout << "Client " << _events[i].data.fd << " sent: " << _readBuffer.data() << std::endl;
				}
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
