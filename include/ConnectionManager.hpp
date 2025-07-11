
#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "ft_irc.hpp"
#include "Connection.hpp"
#include "UserManager.hpp"

class	ConnectionManager
{
	private:
		std::map<int, Connection*>	_connections;
		UserManager					*_userManager;
	public:
		ConnectionManager(const std::string& password);
		~ConnectionManager();

		int			createConnection(int socketFd);
		Connection	*getConnection(int fd);
};

#endif
