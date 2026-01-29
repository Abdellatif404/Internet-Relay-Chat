
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "ft_irc.hpp"
#include "MessageBuffer.hpp"
#include "SendQueue.hpp"

class	Connection
{
	private:
		int					_fd;
		std::string			_address;
		uint16_t			_port;
	public:
		Connection(int fd, strRef address, uint16_t port);
		~Connection();

		int					getFd() const;
		strRef				getAddress() const;
		uint16_t			getPort() const;

		void				receiveData(MessageBuffer *messageBuffer);
		void				sendData(SendQueue *sendQueue);
};

#endif
