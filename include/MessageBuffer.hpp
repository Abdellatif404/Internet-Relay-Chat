
#ifndef	MESSAGE_BUFFER_HPP
#define	MESSAGE_BUFFER_HPP

#include "ft_irc.hpp"

typedef std::vector<char> VectorBuffer;
typedef std::map<int, VectorBuffer> ClientBufferMap;

class	MessageBuffer
{
	private:
		ClientBufferMap	_buffers;
		std::string		_delimiter;

	public:
		MessageBuffer();
		~MessageBuffer();

		void		storeMessage(int fd, const char *data, size_t size);
		std::string	extractMessage(int fd);
};

#endif
