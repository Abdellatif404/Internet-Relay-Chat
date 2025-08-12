
#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include "ft_irc.hpp"
#include "ConnectionManager.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"

class	EventHandler
{
	private:
		static void			_protect(int status, const std::string& errorMsg);
		static void			_processMessage(ChannelManager *chanManager, UserManager *userManager, MessageBuffer *msgBuffer, SendQueue *sendQueue, int fd);
	public:
		static int	initEpoll(std::vector<struct epoll_event> &events);
		static int	waitForEvents(int epollFd, std::vector<struct epoll_event> &events);
		static void newConnection(ConnectionManager *connManager, int serverFd, int epollFd);
		static void clientDisconnection(ConnectionManager *connManager, UserManager *userManager, int epollFd, int eventFd, struct epoll_event &event);
		static void recvFromClient(ChannelManager *chanManager, UserManager *userManager, Connection *conn, MessageBuffer *msgBuffer, SendQueue *sendQueue);
		static void sendToClient(Connection *conn, SendQueue *sendQueue, int epollFd);
};

#endif
