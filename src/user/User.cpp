#include "User.hpp"

// Initialize members in the same order as declared in the header
User::User(int fd) : _fd(fd), _hostname("localhost"), _state(UNREGISTERED), 
                     _authenticated(false), _lastPing(time(NULL)), 
                     _connectionTime(time(NULL)), _isOperator(false), _isAway(false) {}

User::~User() {}

// Getters
int User::getFd() const { return _fd; }
const std::string& User::getNickname() const { return _nickname; }
const std::string& User::getUsername() const { return _username; }
const std::string& User::getRealname() const { return _realname; }
const std::string& User::getHostname() const { return _hostname; }
UserState User::getState() const { return _state; }
bool User::isAuthenticated() const { return _authenticated; }
bool User::isRegistered() const { return _state == REGISTERED; }
time_t User::getLastPing() const { return _lastPing; }
time_t User::getConnectionTime() const { return _connectionTime; }
bool User::isOperator() const { return _isOperator; }
const std::string& User::getAwayMessage() const { return _awayMessage; }
bool User::isAway() const { return _isAway; }

// Setters
void User::setNickname(const std::string& nickname) { _nickname = nickname; }
void User::setUsername(const std::string& username) { _username = username; }
void User::setRealname(const std::string& realname) { _realname = realname; }
void User::setHostname(const std::string& hostname) { _hostname = hostname; }
void User::setState(UserState state) { _state = state; }
void User::setAuthenticated(bool auth) { _authenticated = auth; }
void User::setLastPing(time_t ping) { _lastPing = ping; }
void User::setOperator(bool op) { _isOperator = op; }

void User::setAway(bool away, const std::string& message) {
    _isAway = away;
    _awayMessage = away ? message : "";
}

std::string User::getPrefix() const {
    return _nickname + "!" + _username + "@" + _hostname;
}

// Validation methods
bool User::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 30)
        return false;
    
    // First character must be letter or special character
    if (!std::isalpha(nick[0]) && nick[0] != '[' && nick[0] != ']' && 
        nick[0] != '{' && nick[0] != '}' && nick[0] != '\\' && 
        nick[0] != '|' && nick[0] != '_' && nick[0] != '^')
        return false;
    
    // Rest can be alphanumeric or special characters
    for (size_t i = 1; i < nick.length(); i++) {
        if (!std::isalnum(nick[i]) && nick[i] != '[' && nick[i] != ']' && 
            nick[i] != '{' && nick[i] != '}' && nick[i] != '\\' && 
            nick[i] != '|' && nick[i] != '_' && nick[i] != '^' && nick[i] != '-')
            return false;
    }
    
    return true;
}

bool User::isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 20)
        return false;
    
    // Username should not contain spaces or special IRC characters
    for (size_t i = 0; i < username.length(); i++) {
        if (std::isspace(username[i]) || username[i] == '@' || 
            username[i] == '!' || username[i] == ':')
            return false;
    }
    
    return true;
}

// Utility
void User::updateLastPing() {
    _lastPing = time(NULL);
}

bool User::needsPing() const {
    // Send ping every 60 seconds
    return (time(NULL) - _lastPing) > 60;
}