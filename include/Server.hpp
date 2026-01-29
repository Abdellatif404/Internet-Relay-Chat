
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"
#include "EventLoop.hpp"

class	Server
{
	private:
		int			_serverFd;
		std::string	_serverName;
		std::string	_serverVersion;
		uint16_t	_port;
		std::string	_password;
		time_t		_startTime;
		EventLoop	*_eventLoop;
	public:
		Server(uint16_t port, strRef password);
		~Server();

		void	start();
		void	shutdown();
};

#endif
