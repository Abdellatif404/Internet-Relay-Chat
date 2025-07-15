
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"
#include "EventLoop.hpp"

class	Server
{
	private:
		int			_serverFd;
		uint16_t	_port;
		std::string	_password;

		EventLoop	*_eventLoop;

		void		_protect(int status, const std::string& errorMsg);
	public:
		Server(uint16_t port, const std::string& password);
		~Server();

		void	start();
};

#endif
