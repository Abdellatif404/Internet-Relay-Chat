
#ifndef SENDQUEUE_HPP
#define SENDQUEUE_HPP

#include "ft_irc.hpp"

typedef std::queue<std::string> StringQueue;
typedef std::map<int, StringQueue> ClientQueueMap;

class	EventLoop;

class	SendQueue
{
	private:
		int				_epollFd;
		ClientQueueMap	_clientQueues;
	public:
		SendQueue(int epollFd);
		~SendQueue();

		void		enqueueMessage(int clientFd, strRef message);
		bool		hasQueuedMessages(int clientFd) const;
		std::string	dequeueMessage(int clientFd);
};

#endif
