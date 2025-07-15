#include "EventLoop.hpp"
#include "SocketHandler.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "PrivMsgCommand.hpp"
#include "QuitCommand.hpp"
#include "PingCommand.hpp"

EventLoop::EventLoop(int serverFd, const std::string& password)
	: _srvFd(serverFd), _epFd(-1)
{
	_epFd = epoll_create1(0);
	_protect(_epFd, "Failed to create epoll instance");

	_msgBuffer = new MessageBuffer();
	_sendQueue = new SendQueue(this);
	_connManager = new ConnectionManager();
	_userManager = new UserManager(password, _sendQueue);
	_chanManager = new ChannelManager(_sendQueue);

	_events.resize(1024);
	for (size_t i = 0; i < _events.size(); ++i)
		_events[i].data.fd = -1;
}

EventLoop::~EventLoop()
{
	if (_epFd >= 0)
		close(_epFd);
	delete _connManager;
	delete _userManager;
	delete _chanManager;
	delete _msgBuffer;
	delete _sendQueue;
}

void EventLoop::_protect(int status, const std::string& errorMsg)
{
	if (status < 0)
	{
		if (_epFd >= 0)
			close(_epFd);
		throw std::runtime_error(errorMsg);
	}
}

void EventLoop::addSocket(int fd)
{
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN | EPOLLRDHUP;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epFd, EPOLL_CTL_ADD, fd, &event);
	_protect(exitCode, "Failed to add socket to epoll");
}

void EventLoop::modifySocket(int fd, uint32_t events)
{
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = events;
	event.data.fd = fd;

	int exitCode = epoll_ctl(_epFd, EPOLL_CTL_MOD, fd, &event);
	_protect(exitCode, "Failed to modify socket in epoll");
}

void EventLoop::removeSocket(int fd)
{
	int exitCode = epoll_ctl(_epFd, EPOLL_CTL_DEL, fd, NULL);
	_protect(exitCode, "Failed to remove socket from epoll");
}

void EventLoop::handleEvents()
{
	int		eventCount = epoll_wait(_epFd, _events.data(), _events.size(), -1);
	_protect(eventCount, "Failed to wait for epoll events");

	for (int i = 0; i < eventCount; i++)
	{
		int			eventFd = _events[i].data.fd;
		uint32_t	eventFlags = _events[i].events;

		if (eventFd == _srvFd)
		{
			if (eventFlags & EPOLLIN)
			{
				int connFd = _connManager->createConnection(_srvFd);
				if (connFd < 0)
					continue;
				addSocket(connFd);
			}
		}
		else
		{
			if (eventFd < 0)
				continue;
			if (eventFlags & EPOLLRDHUP)
			{
				_connManager->removeConnection(eventFd);
				_events[i].data.fd = -1;
				_events[i].events = 0;
				removeSocket(eventFd);
				close(eventFd);
				continue;
			}
			if (eventFlags & (EPOLLIN | EPOLLOUT))
			{
				Connection		*conn = _connManager->getConnection(eventFd);
				if (!conn)
					continue;
				if (eventFlags & EPOLLIN) {
					conn->receiveData(_msgBuffer);
					_processUserMessages(eventFd);  // Process user management messages
				}
				if (eventFlags & EPOLLOUT)
				{
					conn->sendData(_sendQueue);
					if (!_sendQueue->hasQueuedMessages(eventFd))
						modifySocket(eventFd, EPOLLIN | EPOLLRDHUP);
				}
			}
		}
	}
}

void EventLoop::run()
{
	addSocket(_srvFd);
	std::cout << GREEN << "Server is running..." << RESET << std::endl;
	while (true)
	{
		handleEvents();
	}
}

// Process messages for User Management commands only
void EventLoop::_processUserMessages(int fd) {
	std::string message;
	while (!(message = _msgBuffer->extractMessage(fd)).empty()) {
		IRCMessage ircMsg = MessageParser::parse(message);
		User* user = _userManager->getUser(fd);
		
		if (!user) {
			// Create user if doesn't exist
			user = _userManager->createUser(fd);
		}
		
		// Process only User Management commands (your part)
		if (ircMsg.command == "PASS") {
			PassCommand::execute(user, ircMsg.params, _userManager);
		}
		else if (ircMsg.command == "NICK") {
			NickCommand::execute(user, ircMsg.params, _userManager);
		}
		else if (ircMsg.command == "USER") {
			UserCommand::execute(user, ircMsg.params, _userManager);
		}
		else if (ircMsg.command == "PRIVMSG") {
			PrivMsgCommand::execute(user, ircMsg.params, _userManager);
		}
		else if (ircMsg.command == "QUIT") {
			QuitCommand::execute(user, ircMsg.params, _userManager);
		}
		else if (ircMsg.command == "PING") {
			PingCommand::execute(user, ircMsg.params, _userManager);
		}
		// TODO: Channel commands (JOIN, PART, etc.) will be handled by channel system
		// TODO: Other server commands will be handled by their respective systems
		else {
			// Unknown command - send error
			if (user->isRegistered()) {
				std::string error = ":localhost 421 " + user->getNickname() + " " + ircMsg.command + " :Unknown command\r\n";
				_userManager->sendMessage(user, error);
			}
		}
	}
}
