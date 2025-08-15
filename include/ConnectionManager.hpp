
#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "ft_irc.hpp"
#include "Connection.hpp"

typedef std::map<int, Connection*> ConnectionMap;

class	ConnectionManager
{
	private:
		ConnectionMap	_connections;
	public:
		ConnectionManager();
		~ConnectionManager();

		Connection		*getConnection(int fd);
		ConnectionMap	&getConnections();
		int				createConnection(int fd);
		void			removeConnection(int fd);
};

#endif
