
#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "ft_irc.hpp"
#include "ConnectionManager.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"
#include "MessageParser.hpp"

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

		void _protect(int status, const std::string& errorMsg);
		void _processUserMessages(int fd);
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
