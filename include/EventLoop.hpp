
#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "ft_irc.hpp"

class EventLoop
{
	private:
		int _epollFd;
		int _serverFd;
		std::vector<struct epoll_event>	_events;
		std::vector<char> 				_readBuffer;

		void _protect(int status, const std::string& errorMsg);
	public:
		EventLoop(int serverFd);
		~EventLoop();

		void run();
		void addSocket(int fd);
		void modifySocket(int fd, int events);
		void removeSocket(int fd);
		void handleEvents();
};

#endif
