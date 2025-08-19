
#include "MessageBuffer.hpp"

MessageBuffer::MessageBuffer()
	: _delimiter("\r\n")
{
}

MessageBuffer::~MessageBuffer()
{
}

ClientBufferMap &MessageBuffer::getBuffers()
{
	return _buffers;
}

void MessageBuffer::storeMessage(int fd, const char *data, size_t size)
{
	_buffers[fd].insert(_buffers[fd].end(), data, data + size);
}

std::string MessageBuffer::extractMessage(int fd)
{
	ClientBufferMap::iterator it = _buffers.find(fd);
	if (it == _buffers.end())
		return "";
	VectorBuffer &buffer = it->second;
	VectorBuffer::iterator delimiterPos = std::search(buffer.begin(), buffer.end(),
		_delimiter.begin(), _delimiter.end());
	if (delimiterPos != buffer.end())
	{
		std::string message(buffer.begin(), delimiterPos);
		buffer.erase(buffer.begin(), delimiterPos + _delimiter.size());
		if (message.size() > 512)
			return "";
		return message;
	}
	return "";
}
