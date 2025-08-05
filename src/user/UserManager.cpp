#include "UserManager.hpp"

UserManager::UserManager(const std::string& password, SendQueue *sendQueue, const std::string& serverName) 
    : _serverPassword(password), _serverName(serverName), _serverVersion("1.0"), 
      _sendQueue(sendQueue), _maxUsers(100) {}

UserManager::UserManager(const std::string& password, SendQueue *sendQueue) 
    : _serverPassword(password), _serverName("localhost"), _serverVersion("1.0"), 
      _sendQueue(sendQueue), _maxUsers(100) {}

UserManager::~UserManager()
{
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it)
        delete it->second;
    _users.clear();
    _nicknames.clear();
}

User* UserManager::createUser(int fd)
{
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end())
        return it->second;
    
    User* user = new User(fd);
    _users[fd] = user;
    return user;
}

void UserManager::removeUser(int fd)
{
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        cleanupNickname(it->second);
        delete it->second;
        _users.erase(it);
    }
}

void UserManager::cleanupNickname(User* user)
{
    if (!user || user->getNickname().empty()) return;
    
    std::string lowerNick = toLowerCase(user->getNickname());
    std::map<std::string, User*>::iterator nickIt = _nicknames.find(lowerNick);
    if (nickIt != _nicknames.end() && nickIt->second == user)
        _nicknames.erase(nickIt);
}

User* UserManager::getUser(int fd) const
{
    std::map<int, User*>::const_iterator it = _users.find(fd);
    return (it != _users.end()) ? it->second : NULL;
}

User* UserManager::getUserByNickname(const std::string& nickname) const
{
    std::map<std::string, User*>::const_iterator it = _nicknames.find(toLowerCase(nickname));
    return (it != _nicknames.end()) ? it->second : NULL;
}

bool UserManager::authenticateUser(User* user, const std::string& password)
{
    if (password == _serverPassword) {
        user->setAuthenticated(true);
        user->setState(PASS_SET);
        return true;
    }
    return false;
}

bool UserManager::isNicknameAvailable(const std::string& nickname) const
{
    return _nicknames.find(toLowerCase(nickname)) == _nicknames.end();
}

bool UserManager::updateNickname(User* user, const std::string& nickname)
{
    if (!user) return false;
    
    std::string lowerNick = toLowerCase(nickname);
    std::map<std::string, User*>::iterator it = _nicknames.find(lowerNick);
    if (it != _nicknames.end() && it->second != user)
        return false;
    
    // Remove old nickname if exists
    cleanupNickname(user);
    
    // Set new nickname
    user->setNickname(nickname);
    _nicknames[lowerNick] = user;
    
    if (user->getState() == PASS_SET)
        user->setState(NICK_SET);
    
    return true;
}

bool UserManager::tryCompleteRegistration(User* user)
{
    if (user->isAuthenticated() && !user->getNickname().empty() && 
        !user->getUsername().empty() && user->getState() == NICK_SET) {
        user->setState(REGISTERED);
        sendWelcomeMessages(user);
        return true;
    }
    return false;
}

void UserManager::sendWelcomeMessages(User* user)
{
    const std::string& nick = user->getNickname();
    const std::string& host = _serverName;
    
    // Welcome sequence
    std::string messages[] = {
        ":" + host + " " + RPL_WELCOME + " " + nick + " :Welcome to the Internet Relay Network " + user->getPrefix(),
        ":" + host + " " + RPL_YOURHOST + " " + nick + " :Your host is " + host + ", running version " + _serverVersion,
        ":" + host + " " + RPL_CREATED + " " + nick + " :This server was created sometime",
        ":" + host + " " + RPL_MYINFO + " " + nick + " " + host + " " + _serverVersion + " oir owimnstkl",
        ":" + host + " " + RPL_ISUPPORT + " " + nick + " CASEMAPPING=rfc1459 CHANLIMIT=#:20 CHANNELLEN=50 CHANTYPES=# ELIST=U EXCEPTS INVEX KICKLEN=307 MAXLIST=bqeI:100 MODES=4 NETWORK=ft_irc :are supported by this server",
        ":" + host + " " + RPL_ISUPPORT + " " + nick + " NICKLEN=30 PREFIX=(oir)@%+ SAFELIST STATUSMSG=@%+ STD=rfc2812 TARGMAX=NAMES:1,LIST:1,KICK:1,WHOIS:1,PRIVMSG:4,NOTICE:4,ACCEPT:,MONITOR: :are supported by this server"
    };
    
    for (size_t i = 0; i < sizeof(messages)/sizeof(messages[0]); ++i)
        _sendQueue->enqueueMessage(user->getFd(), messages[i] + "\r\n");
}

void UserManager::sendMessage(User* user, const std::string& message)
{
    if (_sendQueue && user)
        _sendQueue->enqueueMessage(user->getFd(), message);
}

void UserManager::sendToUser(int fd, const std::string& message)
{
    if (_sendQueue)
        _sendQueue->enqueueMessage(fd, message);
}

void UserManager::sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message)
{
    User* target = getUserByNickname(targetNick);
    if (!target) {
        sendError(sender, 401, targetNick + " :No such nick/channel");
        return;
    }
    
    std::string privmsg = ":" + sender->getPrefix() + " PRIVMSG " + targetNick + " :" + message + "\r\n";
    sendMessage(target, privmsg);
}

void UserManager::broadcastToAll(const std::string& message, User* except)
{
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        if (user != except && user->isRegistered())
            sendMessage(user, message);
    }
}

std::vector<User*> UserManager::getAllUsers() const
{
    std::vector<User*> users;
    users.reserve(_users.size());
    for (std::map<int, User*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
        users.push_back(it->second);
    return users;
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

std::string UserManager::toLowerCase(const std::string& str)
{
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        char& c = result[i];
        if (c >= 'A' && c <= 'Z')
            c += ('a' - 'A');
        else if (c == '[') c = '{';
        else if (c == ']') c = '}';
        else if (c == '\\') c = '|';
        else if (c == '~') c = '^';
    }
    return result;
}

void UserManager::handleUserQuit(User* user, const std::string& quitMessage)
{
    if (!user) return;
    
    std::string message = quitMessage.empty() ? "Client Quit" : quitMessage;
    std::string quitMsg = ":" + user->getPrefix() + " QUIT :" + message + "\r\n";
    broadcastToAll(quitMsg, user);
    removeUser(user->getFd());
}

void UserManager::pingUsers()
{
    std::string pingMsg = ":localhost PING :localhost\r\n";
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        if (user->isRegistered())
            sendMessage(user, pingMsg);
    }
}

void UserManager::disconnectIdleUsers()
{
    const time_t currentTime = time(NULL);
    const int IDLE_TIMEOUT = 300;
    std::vector<int> idleUsers;
    
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        if (!user->isRegistered() && (currentTime - user->getConnectionTime()) > IDLE_TIMEOUT)
            idleUsers.push_back(user->getFd());
    }
    
    for (std::vector<int>::iterator it = idleUsers.begin(); it != idleUsers.end(); ++it)
        removeUser(*it);
}

void UserManager::sendError(User* user, int errorCode, const std::string& errorText)
{
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":localhost " << errorCode << " " << nick;
    if (!errorText.empty())
        oss << " :" << errorText;
    oss << "\r\n";
    sendMessage(user, oss.str());
}

void UserManager::sendNumericReply(User* user, int code, const std::string& text)
{
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":localhost " << code << " " << nick << " " << text << "\r\n";
    sendMessage(user, oss.str());
}
