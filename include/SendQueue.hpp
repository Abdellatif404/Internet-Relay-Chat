
#ifndef SENDQUEUE_HPP
#define SENDQUEUE_HPP

#include "ft_irc.hpp"

typedef std::queue<std::string> StringQueue;
typedef std::map<int, StringQueue> ClientQueueMap;

class	EventLoop;

class	SendQueue
{
	private:
		ClientQueueMap	_clientQueues;
		EventLoop		*_eventLoop;
	public:
		SendQueue(EventLoop *eventLoop);
		~SendQueue();

		void		enqueueMessage(int clientFd, const std::string &message);
		bool		hasQueuedMessages(int clientFd) const;
		std::string	dequeueMessage(int clientFd);
};

#endif
