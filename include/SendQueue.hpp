
#ifndef SENDQUEUE_HPP
#define SENDQUEUE_HPP

#include "ft_irc.hpp"

typedef std::queue<std::string> StringQueue;
typedef std::map<int, StringQueue> ClientQueueMap;

class	SendQueue
{
	private:
		ClientQueueMap	_clientQueues;
	public:
		SendQueue();
		~SendQueue();

		void		enqueueMessage(int clientFd, const std::string &message);
		bool		hasQueuedMessages(int clientFd) const;
		std::string	dequeueMessage(int clientFd);
};

#endif
