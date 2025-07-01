
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class	Server
{
	private:
		int			_port;
		std::string	_password;
		int			_serverFd;

		void		_protect(int status, const std::string& errorMsg);
	public:
		Server(int port, const std::string& password);
		~Server();

		void	start();
};

#endif
