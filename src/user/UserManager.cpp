#include "UserManager.hpp"

UserManager::UserManager(const std::string& password, SendQueue *sendQueue, const std::string& serverName) 
    : _serverPassword(password), _sendQueue(sendQueue), _serverName(serverName), _serverVersion("1.0"), _maxUsers(100) {
    // Three-parameter constructor with explicit server name
}

UserManager::UserManager(const std::string& password, SendQueue *sendQueue) 
    : _serverPassword(password), _sendQueue(sendQueue), _serverName("localhost"), _serverVersion("1.0"), _maxUsers(100) {
    // Constructor overload for EventLoop compatibility - uses default server name
}

UserManager::~UserManager() {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        delete it->second;
    }
    _users.clear();
    _nicknames.clear();
}

User* UserManager::createUser(int fd) {
    User* user = new User(fd);
    _users[fd] = user;
    return user;
}

void UserManager::removeUser(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        User* user = it->second;
        
        // Remove from nickname map
        if (!user->getNickname().empty()) {
            _nicknames.erase(user->getNickname());
        }
        
        delete user;
        _users.erase(it);
    }
}

User* UserManager::getUser(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    return (it != _users.end()) ? it->second : NULL;
}

User* UserManager::getUserByNickname(const std::string& nickname) {
    std::map<std::string, User*>::iterator it = _nicknames.find(nickname);
    return (it != _nicknames.end()) ? it->second : NULL;
}

bool UserManager::authenticateUser(User* user, const std::string& password) {
    if (password == _serverPassword) {
        user->setAuthenticated(true);
        user->setState(PASS_SET);
        return true;
    }
    return false;
}

bool UserManager::isNicknameAvailable(const std::string& nickname) {
    return _nicknames.find(nickname) == _nicknames.end();
}

bool UserManager::registerNickname(User* user, const std::string& nickname) {
    if (!isNicknameAvailable(nickname)) {
        return false;
    }
    
    // Remove old nickname if exists
    if (!user->getNickname().empty()) {
        _nicknames.erase(user->getNickname());
    }
    
    user->setNickname(nickname);
    _nicknames[nickname] = user;
    
    if (user->getState() == PASS_SET) {
        user->setState(NICK_SET);
    }
    
    return true;
}

void UserManager::unregisterNickname(const std::string& nickname) {
    _nicknames.erase(nickname);
}

bool UserManager::tryCompleteRegistration(User* user) {
    if (user->isAuthenticated() && 
        !user->getNickname().empty() && 
        !user->getUsername().empty() &&
        user->getState() == NICK_SET) {
        
        user->setState(USR_REGISTERED);
        sendWelcomeMessages(user);
        return true;
    }
    return false;
}

void UserManager::sendWelcomeMessages(User* user) {
    std::string nick = user->getNickname();
    std::string host = "localhost";
    
    // 001 RPL_WELCOME
    std::string welcome = ":" + host + " 001 " + nick + " :Welcome to the Internet Relay Network " + user->getPrefix() + "\r\n";
    _sendQueue->enqueueMessage(user->getFd(), welcome);
    
    // 002 RPL_YOURHOST
    std::string yourhost = ":" + host + " 002 " + nick + " :Your host is " + host + ", running version 1.0\r\n";
    _sendQueue->enqueueMessage(user->getFd(), yourhost);
    
    // 003 RPL_CREATED
    std::string created = ":" + host + " 003 " + nick + " :This server was created sometime\r\n";
    _sendQueue->enqueueMessage(user->getFd(), created);
    
    // 004 RPL_MYINFO
    std::string myinfo = ":" + host + " 004 " + nick + " " + host + " 1.0 o o\r\n";
    _sendQueue->enqueueMessage(user->getFd(), myinfo);
}

void UserManager::sendMessage(User* user, const std::string& message) {
    if (_sendQueue && user) {
        _sendQueue->enqueueMessage(user->getFd(), message);
    }
}

std::vector<User*> UserManager::getAllUsers() const {
    std::vector<User*> users;
    for (std::map<int, User*>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        users.push_back(it->second);
    }
    return users;
}

size_t UserManager::getUserCount() const {
    return _users.size();
}

bool UserManager::changeNickname(User* user, const std::string& newNickname) {
    if (!isNicknameAvailable(newNickname)) {
        return false;
    }
    
    std::string oldNickname = user->getNickname();
    if (!oldNickname.empty()) {
        _nicknames.erase(oldNickname);
    }
    
    user->setNickname(newNickname);
    _nicknames[newNickname] = user;
    
    return true;
}

void UserManager::sendToUser(int fd, const std::string& message) {
    if (_sendQueue) {
        _sendQueue->enqueueMessage(fd, message);
    }
}

void UserManager::sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message) {
    User* target = getUserByNickname(targetNick);
    if (!target) {
        // 401 ERR_NOSUCHNICK
        std::string error = ":localhost 401 " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        sendMessage(sender, error);
        return;
    }
    
    std::string privmsg = ":" + sender->getPrefix() + " PRIVMSG " + targetNick + " :" + message + "\r\n";
    sendMessage(target, privmsg);
}

void UserManager::broadcastToAll(const std::string& message, User* except) {
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        if (user != except && user->isRegistered()) {
            sendMessage(user, message);
        }
    }
}

void UserManager::setServerOperator(const std::string& nickname) {
    if (std::find(_operators.begin(), _operators.end(), nickname) == _operators.end()) {
        _operators.push_back(nickname);
    }
}

bool UserManager::isServerOperator(const std::string& nickname) const {
    return std::find(_operators.begin(), _operators.end(), nickname) != _operators.end();
}

std::string UserManager::toLowerCase(const std::string& str) const {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + ('a' - 'A');
        }
    }
    return result;
}

void UserManager::handleUserQuit(User* user, const std::string& quitMessage) {
    if (!user) return;
    
    std::string nick = user->getNickname();
    std::string message = quitMessage.empty() ? "Client Quit" : quitMessage;
    
    // Simple quit handling - channel notifications will be handled by ChannelManager
    std::string quitMsg = ":" + user->getPrefix() + " QUIT :" + message + "\r\n";
    
    // For now, just remove the user - channel system will handle notifications
    removeUser(user->getFd());
}

void UserManager::pingUsers() {
    std::string pingMsg = ":localhost PING :localhost\r\n";
    
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        if (user->isRegistered()) {
            sendMessage(user, pingMsg);
            // Update last ping time would be handled in User class
        }
    }
}

void UserManager::disconnectIdleUsers() {
    time_t currentTime = time(NULL);
    const int IDLE_TIMEOUT = 300; // 5 minutes
    
    std::vector<int> idleUsers;
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        User* user = it->second;
        // This would need lastActivity tracking in User class
        // For now, just check if user is not registered for too long
        if (!user->isRegistered() && (currentTime - user->getConnectionTime()) > IDLE_TIMEOUT) {
            idleUsers.push_back(user->getFd());
        }
    }
    
    for (std::vector<int>::iterator it = idleUsers.begin(); it != idleUsers.end(); ++it) {
        removeUser(*it);
    }
}

void UserManager::sendError(User* user, int errorCode, const std::string& errorText) {
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":localhost " << errorCode << " " << nick;
    if (!errorText.empty()) {
        oss << " :" << errorText;
    }
    oss << "\r\n";
    sendMessage(user, oss.str());
}

void UserManager::sendNumericReply(User* user, int code, const std::string& text) {
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    std::ostringstream oss;
    oss << ":localhost " << code << " " << nick << " " << text << "\r\n";
    sendMessage(user, oss.str());
}