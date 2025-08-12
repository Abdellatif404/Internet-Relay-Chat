
#ifndef SOCKET_HANDLER_HPP
#define SOCKET_HANDLER_HPP

#include "ft_irc.hpp"

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
		static void			addSocket(int epFd, int fd);
		static void			modifySocket(int epFd, int fd, uint32_t events);
		static void			removeSocket(int epFd, int fd);
};

#endif
