
#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "ft_irc.hpp"
#include "Connection.hpp"

typedef std::map<int, Connection*> ConnectionMap;

class	ConnectionManager
{
	private:
		ConnectionMap	_connections;
		MessageBuffer	*_msgBuffer;
		SendQueue		*_sendQueue;
		eventVec		_events;
		int				_epollFd;
	public:
		ConnectionManager(MessageBuffer *msgBuffer, SendQueue *sendQueue, eventVec &events, int epollFd);
		~ConnectionManager();

		Connection		*getConnection(int fd);
		ConnectionMap	&getConnections();
		int				createConnection(int fd);
		void			removeConnection(int fd);
};

#endif
