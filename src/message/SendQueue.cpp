
#include "SendQueue.hpp"
#include "SocketHandler.hpp"

SendQueue::SendQueue(int epollFd)
	: _epollFd(epollFd)
{
};

SendQueue::~SendQueue()
{
};

ClientQueueMap &SendQueue::getClientQueues()
{
	return _clientQueues;
}

void SendQueue::enqueueMessage(int clientFd, strRef message)
{
	_clientQueues[clientFd].push(message);
	SocketHandler::modifySocket(_epollFd, clientFd, EPOLLIN | EPOLLOUT);
}

bool SendQueue::hasQueuedMessages(int clientFd) const
{
	ClientQueueMap::const_iterator it = _clientQueues.find(clientFd);
	return it != _clientQueues.end() && !it->second.empty();
}

std::string SendQueue::dequeueMessage(int clientFd)
{
	ClientQueueMap::iterator it = _clientQueues.find(clientFd);
	if (it != _clientQueues.end() && !it->second.empty())
	{
		std::string message = it->second.front();
		it->second.pop();
		return message;
	}
	return "";
}
