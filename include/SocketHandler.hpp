
#ifndef SOCKET_HANDLER_HPP
#define SOCKET_HANDLER_HPP

#include "ft_irc.hpp"
#include "Connection.hpp"
#include "UserManager.hpp"

class SocketHandler
{
	private:
		static void			_protect(int status, const std::string& errorMsg);

	public:
		static int			createSocket();
		static void			setSocketOptions(int fd);
		static void			setNonBlocking(int fd);
		static void			bindSocket(int fd, uint16_t port);
		static void			listenOnSocket(int fd);
		static int			acceptConnection(int fd, soaddr_t *addr);
};

#endif
