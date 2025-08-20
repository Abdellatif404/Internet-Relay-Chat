#include "UserManager.hpp"

UserManager::UserManager(const std::string& password, SendQueue *sendQueue, const std::string& serverName, const std::string& serverVersion) : _serverPassword(password), _sendQueue(sendQueue), _serverName(serverName), _serverVersion(serverVersion), _maxUsers(100),_periodicStatsEnabled(false), _lastStatsTime(time(NULL))
{}

UserManager::UserManager(const std::string& password, SendQueue *sendQueue) : _serverPassword(password), _sendQueue(sendQueue), _serverName("localhost"), _serverVersion("1.0"), _maxUsers(100),_periodicStatsEnabled(false), _lastStatsTime(time(NULL))
{
}

UserManager::~UserManager()
{
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
        if (it->second)
            delete it->second;
    _users.clear();
    _nicknames.clear();
}

User* UserManager::createUser(int fd)
{
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end())
        return it->second;
    try
	{
        User* user = new User(fd);
        _users[fd] = user;
        return user;
    }
	catch (const std::exception& e)
	{
        _users.erase(fd);
        throw;
    }
}

void UserManager::removeUser(int fd)
{
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it == _users.end())
        return;
    User* user = it->second;
    if (!user->getNickname().empty())
    {
        std::string lowerNick = toLowerCase(user->getNickname());
        std::map<std::string, User*>::iterator nickIt = _nicknames.find(lowerNick);
        if (nickIt != _nicknames.end() && nickIt->second == user)
            _nicknames.erase(nickIt);
    }
    _users.erase(it);
    delete user;
}

User* UserManager::getUser(int fd)
{
    std::map<int, User*>::iterator it = _users.find(fd);
    return (it != _users.end()) ? it->second : NULL;
}

User* UserManager::getUserByNickname(const std::string& nickname)
{
    std::map<std::string, User*>::iterator it = _nicknames.find(toLowerCase(nickname));
    return (it != _nicknames.end()) ? it->second : NULL;
}

bool UserManager::authenticateUser(User* user, const std::string& password)
{
    if (password == _serverPassword)
	{
        user->setAuthenticated(true);
        user->setState(PASS_SET);
        return true;
    }
    return false;
}

bool UserManager::isNicknameAvailable(const std::string& nickname)
{
    return _nicknames.find(toLowerCase(nickname)) == _nicknames.end();
}

bool UserManager::registerNickname(User* user, const std::string& nickname)
{
    if (!user)
        return false;
    std::string lowerNick = toLowerCase(nickname);
    std::map<std::string, User*>::iterator it = _nicknames.find(lowerNick);
    if (it != _nicknames.end() && it->second != user)
        return false;
    std::string oldNickname = user->getNickname();
    if (!oldNickname.empty())
        _nicknames.erase(toLowerCase(oldNickname));
    user->setNickname(nickname);
    _nicknames[lowerNick] = user;
    if (user->getState() == PASS_SET)
        user->setState(NICK_SET);
    return true;
}

void UserManager::unregisterNickname(const std::string& nickname)
{
    _nicknames.erase(toLowerCase(nickname));
}

bool UserManager::tryCompleteRegistration(User* user)
{
    if (user->isAuthenticated() && !user->getNickname().empty() && !user->getUsername().empty() && user->getState() == NICK_SET)
	{    
        user->setState(REGISTERED);
        sendWelcomeMessages(user);
        return true;
    }
    return false;
}

void UserManager::sendWelcomeMessages(User* user)
{
    std::string nick = user->getNickname();
    sendMessage(user, ":" + _serverName + " 001 " + nick + " :Welcome to the Internet Relay Network " + user->getPrefix() + "\r\n");
    sendMessage(user, ":" + _serverName + " 002 " + nick + " :Your host is " + _serverName + ", running version " + _serverVersion + "\r\n");
    sendMessage(user, ":" + _serverName + " 003 " + nick + " :This server was created sometime\r\n");
    sendMessage(user, ":" + _serverName + " 004 " + nick + " " + _serverName + " " + _serverVersion + " oir owimnstkl\r\n");
    sendMessage(user, ":" + _serverName + " 005 " + nick + " CASEMAPPING=rfc1459 CHANLIMIT=#:20 CHANNELLEN=50 CHANTYPES=# ELIST=U EXCEPTS INVEX KICKLEN=307 MAXLIST=bqeI:100 MODES=4 NETWORK=ft_irc :are supported by this server\r\n");
    sendMessage(user, ":" + _serverName + " 005 " + nick + " NICKLEN=30 PREFIX=(oir)@%+ SAFELIST STATUSMSG=@%+ STD=rfc2812 TARGMAX=NAMES:1,LIST:1,KICK:1,WHOIS:1,PRIVMSG:4,NOTICE:4,ACCEPT:,MONITOR: :are supported by this server\r\n");
}

void UserManager::sendMessage(User* user, const std::string& message)
{
    if (_sendQueue && user)
        _sendQueue->enqueueMessage(user->getFd(), message);
}

std::vector<User*> UserManager::getAllUsers() const
{
    std::vector<User*> users;
    for (std::map<int, User*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
        users.push_back(it->second);
    return users;
}

size_t UserManager::getUserCount() const
{
    return _users.size();
}

bool UserManager::changeNickname(User* user, const std::string& newNickname)
{
    if (!user)
        return false;
    std::string newLowerNick = toLowerCase(newNickname);
    std::map<std::string, User*>::iterator it = _nicknames.find(newLowerNick);
    if (it != _nicknames.end() && it->second != user)
        return false;
    std::string oldNickname = user->getNickname();
    if (!oldNickname.empty())
        _nicknames.erase(toLowerCase(oldNickname));
    user->setNickname(newNickname);
    _nicknames[newLowerNick] = user;
    return true;
}

void UserManager::sendToUser(int fd, const std::string& message)
{
    if (_sendQueue)
        _sendQueue->enqueueMessage(fd, message);
}

void UserManager::sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message)
{
    User* target = getUserByNickname(targetNick);
    if (!target)
	{
        std::string error = ":" + _serverName + " " + std::string(ERR_NOSUCHNICK) + " " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        sendMessage(sender, error);
        return;
    }
    std::string privmsg = ":" + sender->getPrefix() + " PRIVMSG " + targetNick + " :" + message + "\r\n";
    sendMessage(target, privmsg);
}

void UserManager::broadcastToAll(const std::string& message, User* except)
{
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
        User* user = it->second;
        if (user != except && user->isRegistered())
            sendMessage(user, message);
    }
}

void UserManager::setServerOperator(const std::string& nickname)
{
    if (std::find(_operators.begin(), _operators.end(), nickname) == _operators.end())
        _operators.push_back(nickname);
}

bool UserManager::isServerOperator(const std::string& nickname) const
{
    return std::find(_operators.begin(), _operators.end(), nickname) != _operators.end();
}

std::string UserManager::toLowerCase(const std::string& str) const
{
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i)
	{
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] + ('a' - 'A');
        else if (result[i] == '[')
            result[i] = '{';
        else if (result[i] == ']')
            result[i] = '}';
        else if (result[i] == '\\')
            result[i] = '|';
        else if (result[i] == '~')
            result[i] = '^';
    }
    return result;
}

void UserManager::handleUserQuit(User* user, ConnectionManager *connManager, ChannelManager *chanManager, const std::string& quitMessage)
{
	(void) quitMessage;
  if (!user)
		return;
  int fd = user->getFd();
	chanManager->removeUserFromAllChannels(user);
    removeUser(fd);
	connManager->removeConnection(fd);
}

void UserManager::pingUsers()
{
    std::string pingMsg = ":" + _serverName + " PING :" + _serverName + "\r\n";
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
        User* user = it->second;
        if (user->isRegistered())
            sendMessage(user, pingMsg);
    }
}

void UserManager::disconnectIdleUsers()
{
    time_t currentTime = time(NULL);
    const int IDLE_TIMEOUT = 300;
    std::vector<int> idleUsers;
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
        User* user = it->second;
        if (user && !user->isRegistered() && (currentTime - user->getConnectionTime()) > IDLE_TIMEOUT)
            idleUsers.push_back(user->getFd());
    }
    for (std::vector<int>::iterator it = idleUsers.begin(); it != idleUsers.end(); ++it)
        removeUser(*it);
}

void UserManager::sendError(User* user, int errorCode, const std::string& errorText)
{
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":" << _serverName << " " << errorCode << " " << nick;
    if (!errorText.empty())
        oss << " :" << errorText;
    oss << "\r\n";
    sendMessage(user, oss.str());
}

void UserManager::sendNumericReply(User* user, int code, const std::string& text)
{
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":" << _serverName << " " << code << " " << nick << " " << text << "\r\n";
    sendMessage(user, oss.str());
}

void UserManager::handleDisconnection(int fd)
{
    User* user = getUser(fd);
    if (user)
	{
        if (user->isRegistered())
            std::string quitMsg = ":" + user->getPrefix() + " QUIT :Connection closed\r\n";
        removeUser(fd);
    }
}

void UserManager::checkAndPrintStats()
{
    if (!_periodicStatsEnabled)
		return;
    time_t currentTime = time(NULL);
    if (currentTime - _lastStatsTime >= 30)
        _lastStatsTime = currentTime;
}