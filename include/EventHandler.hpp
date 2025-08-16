
#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

#include "ft_irc.hpp"
#include "ConnectionManager.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"
#include "BotManager.hpp"

class	EventHandler
{
	private:
		static void			_protect(int status, strRef errorMsg);
		static void			_processMessage(ChannelManager *chanManager, UserManager *userManager, BotManager *botManager, MessageBuffer *msgBuffer, SendQueue *sendQueue, int fd);
	public:
		static int	initEpoll(eventVec &events);
		static int	waitForEvents(int epollFd, eventVec &events);
		static void newConnection(ConnectionManager *connManager, int serverFd, int epollFd);
		static void clientDisconnection(ConnectionManager *connManager, UserManager *userManager, int epollFd, int eventFd, event_t &event);
		static void recvFromClient(ChannelManager *chanManager, UserManager *userManager, BotManager *botManager, Connection *conn, MessageBuffer *msgBuffer, SendQueue *sendQueue);
		static void sendToClient(Connection *conn, SendQueue *sendQueue, int epollFd);
};

#endif
