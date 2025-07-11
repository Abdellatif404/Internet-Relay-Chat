
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "ft_irc.hpp"
#include "User.hpp"

class	Connection
{
	private:
		int					_fd;
		std::string			_address;
		uint16_t			_port;
		User				*_user;
		std::vector<char>	_recvBuffer;
	public:
		Connection(int fd, const std::string& address, uint16_t port);
		~Connection();

		int					getFd() const;
		const std::string&	getAddress() const;
		uint16_t			getPort() const;
		User				*getUser() const;
		void				setUser(User *user);
		void				receiveData();
};

#endif
