
#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "ft_irc.hpp"
#include "ConnectionManager.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"

typedef const std::string& strRef;

class	EventLoop
{
	private:
		int _srvFd;
		int _epFd;
		std::vector<struct epoll_event>	_events;
		ConnectionManager				*_connManager;
		UserManager						*_userManager;
		ChannelManager					*_chanManager;
		MessageBuffer					*_msgBuffer;
		SendQueue						*_sendQueue;
	public:
		EventLoop(int serverFd, strRef pass, strRef srvName, strRef srvVersion, time_t startTime);
		~EventLoop();

		void handleEvents();
		void run();
		void stop();
};

#endif
