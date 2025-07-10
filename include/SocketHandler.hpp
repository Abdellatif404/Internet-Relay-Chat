
#ifndef SOCKET_HANDLER_HPP
#define SOCKET_HANDLER_HPP

#include "ft_irc.hpp"

class SocketHandler
{
	private:
		static void			_protect(int status, const std::string& errorMsg);

	public:
		static int			createSocket();
		static void			setSocketOptions(int socketFd);
		static void			setNonBlocking(int socketFd);
		static void			bindSocket(int socketFd, uint16_t port);
		static void			listenOnSocket(int socketFd);
		static int			acceptConnection(int socketFd, soaddr_t *addr);
};

#endif
