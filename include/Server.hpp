
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class	Server
{
	private:
		int			_port;
		std::string	_password;
	public:
		Server(int port, const std::string& password);
		~Server();

		void	start();
};

#endif
