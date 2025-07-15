
#include "SendQueue.hpp"
#include "EventLoop.hpp"

SendQueue::SendQueue(EventLoop *eventLoop)
	: _eventLoop(eventLoop)
{
};

SendQueue::~SendQueue()
{
};

void SendQueue::enqueueMessage(int clientFd, const std::string &message)
{
	_clientQueues[clientFd].push(message);
	_eventLoop->modifySocket(clientFd, EPOLLIN | EPOLLOUT | EPOLLRDHUP);
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
