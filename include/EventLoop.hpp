
#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "ft_irc.hpp"

class	Connection;

class	EventLoop
{
	private:
		int _epollFd;
		int _serverFd;
		std::vector<struct epoll_event>	_events;
		std::vector<char> 				_readBuffer;
		std::map<int, Connection*>		_connections;

		void _protect(int status, const std::string& errorMsg);
	public:
		EventLoop(int serverFd);
		~EventLoop();

		void addSocket(int fd);
		void modifySocket(int fd, uint32_t events);
		void removeSocket(int fd);
		void handleEvents();
		void run();
};

#endif
