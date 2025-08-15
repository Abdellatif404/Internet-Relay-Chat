#include "User.hpp"
User::User(int fd) : _fd(fd), _hostname("localhost"), _state(UNREGISTERED), 
                     _authenticated(false), _lastPing(time(NULL)), 
                     _connectionTime(time(NULL)), _isOperator(false), _isAway(false) {}

User::~User() {
    
    
}
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
    std::string username = _username.empty() ? "unknown" : _username;
    std::string hostname = _hostname.empty() ? "localhost" : _hostname;
    return _nickname + "!" + username + "@" + hostname;
}
bool User::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 30)  
        return false;
    
    
    
    for (size_t i = 0; i < nick.length(); i++) {
        char c = nick[i];
        
        if (c == ' ' || c == ',' || c == '*' || c == '?' || 
            c == '!' || c == '@' || c == '\r' || c == '\n' || c == '\0')
            return false;
    }
    
    
    char firstChar = nick[0];
    
    if (firstChar == '$' || firstChar == ':' || firstChar == '#' || 
        firstChar == '&' || firstChar == '+')
        return false;
    
    
    
    if (!std::isalpha(firstChar) && firstChar != '[' && firstChar != ']' && 
        firstChar != '\\' && firstChar != '`' && firstChar != '_' && 
        firstChar != '^' && firstChar != '{' && firstChar != '|' && firstChar != '}')
        return false;
    
    
    for (size_t i = 1; i < nick.length(); i++) {
        char c = nick[i];
        if (!std::isalnum(c) && c != '[' && c != ']' && 
            c != '\\' && c != '`' && c != '_' && 
            c != '^' && c != '{' && c != '|' && 
            c != '}' && c != '-')
            return false;
    }
    
    return true;
}

bool User::isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 20)
        return false;
    
    
    for (size_t i = 0; i < username.length(); i++) {
        if (std::isspace(username[i]) || username[i] == '@' || 
            username[i] == '!' || username[i] == ':')
            return false;
    }
    
    return true;
}
void User::updateLastPing() {
    _lastPing = time(NULL);
}

bool User::needsPing() const {
    
    return (time(NULL) - _lastPing) > 60;
}