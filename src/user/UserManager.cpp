#include "UserManager.hpp"

UserManager::UserManager(const std::string& password, SendQueue *sendQueue, const std::string& serverName) 
    : _serverPassword(password), _sendQueue(sendQueue), _serverName(serverName), _serverVersion("1.0"), _maxUsers(100),
      _periodicStatsEnabled(false), _lastStatsTime(time(NULL)) {
    // Three-parameter constructor with explicit server name
}

UserManager::UserManager(const std::string& password, SendQueue *sendQueue) 
    : _serverPassword(password), _sendQueue(sendQueue), _serverName("localhost"), _serverVersion("1.0"), _maxUsers(100),
      _periodicStatsEnabled(false), _lastStatsTime(time(NULL)) {
    // Constructor overload for EventLoop compatibility - uses default server name
}

UserManager::~UserManager() {
    // Clean up all users
    for (std::map<int, User*>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second) { // Null check for safety
            delete it->second;
        }
    }
    _users.clear();
    _nicknames.clear(); // This should already be empty after user cleanup, but clear for safety
}

User* UserManager::createUser(int fd) {
    // Check if user already exists to avoid duplicates
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        return it->second; // User already exists, return existing one
    }
    
    try {
        User* user = new User(fd);
        _users[fd] = user;
        return user;
    } catch (const std::exception& e) {
        // If allocation fails, ensure we don't have dangling entries
        _users.erase(fd);
        throw;
    }
}

void UserManager::removeUser(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    if (it != _users.end()) {
        User* user = it->second;
        
        // Remove from nickname map first (before deleting user)
        if (!user->getNickname().empty()) {
            std::string lowerNick = toLowerCase(user->getNickname());
            std::map<std::string, User*>::iterator nickIt = _nicknames.find(lowerNick);
            if (nickIt != _nicknames.end() && nickIt->second == user) {
                _nicknames.erase(nickIt);
            }
        }
        
        // Remove from users map
        _users.erase(it);
        
        // Delete the user object last
        delete user;
    }
}

User* UserManager::getUser(int fd) {
    std::map<int, User*>::iterator it = _users.find(fd);
    return (it != _users.end()) ? it->second : NULL;
}

User* UserManager::getUserByNickname(const std::string& nickname) {
    std::map<std::string, User*>::iterator it = _nicknames.find(toLowerCase(nickname));
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
    return _nicknames.find(toLowerCase(nickname)) == _nicknames.end();
}

bool UserManager::registerNickname(User* user, const std::string& nickname) {
    if (!user) return false;
    
    std::string lowerNick = toLowerCase(nickname);
    
    // Check if nickname is available (but allow user to re-register same nick)
    std::map<std::string, User*>::iterator it = _nicknames.find(lowerNick);
    if (it != _nicknames.end() && it->second != user) {
        return false;
    }
    
    std::string oldNickname = user->getNickname();
    if (!oldNickname.empty()) {
        std::string oldLowerNick = toLowerCase(oldNickname);
        std::map<std::string, User*>::iterator oldIt = _nicknames.find(oldLowerNick);
        if (oldIt != _nicknames.end() && oldIt->second == user) {
            _nicknames.erase(oldIt);
        }
    }
    
    user->setNickname(nickname);
    _nicknames[lowerNick] = user;
    
    if (user->getState() == PASS_SET) {
        user->setState(NICK_SET);
    }
    
    return true;
}

void UserManager::unregisterNickname(const std::string& nickname) {
    _nicknames.erase(toLowerCase(nickname));
}

bool UserManager::tryCompleteRegistration(User* user) {
    if (user->isAuthenticated() && 
        !user->getNickname().empty() && 
        !user->getUsername().empty() &&
        user->getState() == NICK_SET) {
        
        user->setState(REGISTERED);
        sendWelcomeMessages(user);
        return true;
    }
    return false;
}

void UserManager::sendWelcomeMessages(User* user) {
    std::string nick = user->getNickname();
    std::string host = "localhost";
    
    // 001 RPL_WELCOME
    std::string welcome = ":" + host + " " + RPL_WELCOME + " " + nick + " :Welcome to the Internet Relay Network " + user->getPrefix() + "\r\n";
    _sendQueue->enqueueMessage(user->getFd(), welcome);
    
    // 002 RPL_YOURHOST
    std::string yourhost = ":" + host + " " + RPL_YOURHOST + " " + nick + " :Your host is " + host + ", running version 1.0\r\n";
    _sendQueue->enqueueMessage(user->getFd(), yourhost);
    
    // 003 RPL_CREATED
    std::string created = ":" + host + " " + RPL_CREATED + " " + nick + " :This server was created sometime\r\n";
    _sendQueue->enqueueMessage(user->getFd(), created);
    
    // 004 RPL_MYINFO
    std::string myinfo = ":" + host + " " + RPL_MYINFO + " " + nick + " " + host + " 1.0 oir owimnstkl\r\n";
    _sendQueue->enqueueMessage(user->getFd(), myinfo);
    
    // 005 RPL_ISUPPORT - Modern IRC server capabilities
    std::string isupport1 = ":" + host + " " + RPL_ISUPPORT + " " + nick + " CASEMAPPING=rfc1459 CHANLIMIT=#:20 CHANNELLEN=50 CHANTYPES=# ELIST=U EXCEPTS INVEX";
    isupport1 += " KICKLEN=307 MAXLIST=bqeI:100 MODES=4 NETWORK=ft_irc :are supported by this server\r\n";
    _sendQueue->enqueueMessage(user->getFd(), isupport1);
    
    std::string isupport2 = ":" + host + " " + RPL_ISUPPORT + " " + nick + " NICKLEN=30 PREFIX=(oir)@%+ SAFELIST STATUSMSG=@%+ STD=rfc2812";
    isupport2 += " TARGMAX=NAMES:1,LIST:1,KICK:1,WHOIS:1,PRIVMSG:4,NOTICE:4,ACCEPT:,MONITOR: :are supported by this server\r\n";
    _sendQueue->enqueueMessage(user->getFd(), isupport2);
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
    if (!user) return false;
    
    std::string newLowerNick = toLowerCase(newNickname);
    
    // Check if new nickname is available (but allow user to keep same nick)
    std::map<std::string, User*>::iterator it = _nicknames.find(newLowerNick);
    if (it != _nicknames.end() && it->second != user) {
        return false; // Nickname taken by another user
    }
    
    std::string oldNickname = user->getNickname();
    if (!oldNickname.empty()) {
        std::string oldLowerNick = toLowerCase(oldNickname);
        std::map<std::string, User*>::iterator oldIt = _nicknames.find(oldLowerNick);
        if (oldIt != _nicknames.end() && oldIt->second == user) {
            _nicknames.erase(oldIt);
        }
    }
    
    user->setNickname(newNickname);
    _nicknames[newLowerNick] = user;
    
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
        std::string error = ":localhost " + std::string(ERR_NOSUCHNICK) + " " + sender->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
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
        // RFC 2812: IRC case mapping
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + ('a' - 'A');
        } else if (result[i] == '[') {
            result[i] = '{';
        } else if (result[i] == ']') {
            result[i] = '}';
        } else if (result[i] == '\\') {
            result[i] = '|';
        } else if (result[i] == '~') {
            result[i] = '^';
        }
    }
    return result;
}

void UserManager::handleUserQuit(User* user, const std::string& quitMessage) {
    if (!user) return;
    
    // Check if user still exists in our map to avoid double-deletion
    std::map<int, User*>::iterator it = _users.find(user->getFd());
    if (it == _users.end()) {
        return; // User already removed
    }
    
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
        if (user && !user->isRegistered() && (currentTime - user->getConnectionTime()) > IDLE_TIMEOUT) {
            idleUsers.push_back(user->getFd());
        }
    }
    
    // Remove idle users in separate loop to avoid iterator invalidation
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

void UserManager::handleDisconnection(int fd) {
    User* user = getUser(fd);
    if (user) {
        // Notify other users about the disconnection if user was registered
        if (user->isRegistered()) {
            std::string quitMsg = ":" + user->getPrefix() + " QUIT :Connection closed\r\n";
            // TODO: Notify channels when channel system is integrated
        }
        
        // Clean up the user
        removeUser(fd);
    }
}

void UserManager::printMemoryStats() const {
    std::cout << "=== UserManager Memory Statistics ===" << std::endl;
    std::cout << "Active users: " << _users.size() << std::endl;
    std::cout << "Active nicknames: " << _nicknames.size() << std::endl;
    std::cout << "Server operators: " << _operators.size() << std::endl;
    
    // Check for consistency between maps
    if (_users.size() != _nicknames.size()) {
        std::cout << "⚠️  WARNING: User/Nickname map size mismatch!" << std::endl;
    }
    
    // Count registered vs unregistered users
    int registered = 0, unregistered = 0;
    for (std::map<int, User*>::const_iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->second) {
            if (it->second->isRegistered()) {
                registered++;
            } else {
                unregistered++;
            }
        }
    }
    
    std::cout << "Registered users: " << registered << std::endl;
    std::cout << "Unregistered users: " << unregistered << std::endl;
    std::cout << "========================================" << std::endl;
}

void UserManager::checkAndPrintStats() {
    if (!_periodicStatsEnabled) return;
    
    time_t currentTime = time(NULL);
    // Print stats every 30 seconds
    if (currentTime - _lastStatsTime >= 30) {
        printMemoryStats();
        _lastStatsTime = currentTime;
    }
}