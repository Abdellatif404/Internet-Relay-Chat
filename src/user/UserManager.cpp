#include "UserManager.hpp"
#include <iostream>
#include <sys/socket.h>

UserManager::UserManager(const std::string& password) : _serverPassword(password) {}

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
    std::string welcome = ":" + host + " 001 " + nick + " :Welcome to the Internet Relay Network " + user->getPrefix() + "\r\n";
    sendMessage(user, welcome);
    
    // 002 RPL_YOURHOST
    std::string yourhost = ":" + host + " 002 " + nick + " :Your host is " + host + ", running version 1.0\r\n";
    sendMessage(user, yourhost);
    
    // 003 RPL_CREATED
    std::string created = ":" + host + " 003 " + nick + " :This server was created sometime\r\n";
    sendMessage(user, created);
    
    // 004 RPL_MYINFO
    std::string myinfo = ":" + host + " 004 " + nick + " " + host + " 1.0 o o\r\n";
    sendMessage(user, myinfo);
}

void UserManager::sendMessage(User* user, const std::string& message) {
    send(user->getFd(), message.c_str(), message.length(), 0);
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