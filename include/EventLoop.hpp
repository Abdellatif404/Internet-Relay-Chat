
#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "ft_irc.hpp"
#include "ConnectionManager.hpp"
#include "ChannelManager.hpp"

class	EventLoop
{
	private:
		int _epollFd;
		int _serverFd;
		std::vector<struct epoll_event>	_events;
		ConnectionManager				*_connectionManager;
		ChannelManager					*_channelManager;

		void _protect(int status, const std::string& errorMsg);
	public:
		EventLoop(int serverFd, const std::string& password);
		~EventLoop();

		void addSocket(int fd);
		void modifySocket(int fd, uint32_t events);
		void removeSocket(int fd);
		void handleEvents();
		void run();
};

#endif
