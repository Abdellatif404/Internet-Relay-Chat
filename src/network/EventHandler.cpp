
#include "EventHandler.hpp"
#include "SocketHandler.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "PrivMsgCommand.hpp"
#include "QuitCommand.hpp"
#include "PingCommand.hpp"
#include "JoinCommand.hpp"
#include "PartCommand.hpp"
#include "TopicCommand.hpp"
#include "ModeCommand.hpp"
#include "KickCommand.hpp"
#include "InviteCommand.hpp"
#include "BotCommand.hpp"
#include "MessageParser.hpp"

void EventHandler::_protect(int status, strRef errorMsg)
{
	if (status < 0)
		throw std::runtime_error(errorMsg);
}

void EventHandler::_processMessage(ChannelManager *chanManager, UserManager *userManager, BotManager *botManager, MessageBuffer *msgBuffer, SendQueue *sendQueue, int fd)
{
	std::string message;
	while (!(message = msgBuffer->extractMessage(fd)).empty())
	{
		IRCMessage ircMsg = MessageParser::parse(message);
		User* user = userManager->getUser(fd);
		
		if (!user)
			user = userManager->createUser(fd);

		if (ircMsg.command == "PASS")
			PassCommand::execute(user, ircMsg.params, userManager);
		else if (ircMsg.command == "NICK")
			NickCommand::execute(user, ircMsg.params, userManager);
		else if (ircMsg.command == "USER")
			UserCommand::execute(user, ircMsg.params, userManager);
		else if (ircMsg.command == "PRIVMSG")
			PrivMsgCommand::execute(user, ircMsg.params, userManager, chanManager, botManager);
		else if (ircMsg.command == "QUIT")
			QuitCommand::execute(user, ircMsg.params, userManager);
		else if (ircMsg.command == "PING")
			PingCommand::execute(user, ircMsg.params, userManager);
		else if (ircMsg.command == "JOIN") {
			JoinCommand joinCmd(chanManager, sendQueue);
			joinCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "PART") {
			PartCommand partCmd(chanManager, sendQueue);
			partCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "TOPIC") {
			TopicCommand topicCmd(chanManager, sendQueue);
			topicCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "MODE") {
			ModeCommand modeCmd(chanManager, userManager, sendQueue);
			modeCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "KICK") {
			KickCommand kickCmd(chanManager, userManager, sendQueue);
			kickCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "INVITE") {
			InviteCommand inviteCmd(chanManager, userManager, sendQueue);
			inviteCmd.execute(user, ircMsg.params);
		}
		else if (ircMsg.command == "BOT") {
			BotCommand::execute(user, ircMsg.params, botManager, userManager);
		}
		else
		{
			if (user->isRegistered())
			{
				std::string error = ":localhost 421 " + user->getNickname() + " " + ircMsg.command + " :Unknown command\r\n";
				userManager->sendMessage(user, error);
			}
		}
	}
}

int EventHandler::initEpoll(eventVec &events)
{
	int fd = epoll_create1(0);
	_protect(fd, "Failed to create epoll instance");
	events.resize(1024);
	for (size_t i = 0; i < events.size(); ++i)
		events[i].data.fd = -1;
	return fd;
}

int EventHandler::waitForEvents(int epollFd, eventVec &events)
{
	int eventCount = epoll_wait(epollFd, events.data(), events.size(), -1);
	_protect(eventCount, "Failed to wait for epoll events");
	return eventCount;
}

void EventHandler::newConnection(ConnectionManager *connManager, int serverFd, int epollFd)
{
	int fd = connManager->createConnection(serverFd);
	if (fd < 0)
		return;
	SocketHandler::addSocket(epollFd, fd);
}

void EventHandler::clientDisconnection(ConnectionManager *connManager, UserManager *userManager, int epollFd, int eventFd, event_t &event)
{
	connManager->removeConnection(eventFd);
	userManager->removeUser(eventFd);
	event.data.fd = -1;
	event.events = 0;
	SocketHandler::removeSocket(epollFd, eventFd);
	close(eventFd);
}

void EventHandler::recvFromClient(ChannelManager *chanManager, UserManager *userManager, BotManager *botManager, Connection *conn, MessageBuffer *msgBuffer, SendQueue *sendQueue)
{
	if (!conn || !msgBuffer)
		return;
	conn->receiveData(msgBuffer);
	_processMessage(chanManager, userManager, botManager, msgBuffer, sendQueue, conn->getFd());
}

void EventHandler::sendToClient(Connection *conn, SendQueue *sendQueue, int epollFd)
{
	if (!conn || !sendQueue)
		return;
	conn->sendData(sendQueue);
	if (!sendQueue->hasQueuedMessages(conn->getFd()))
		SocketHandler::modifySocket(epollFd, conn->getFd(), EPOLLIN);
}
