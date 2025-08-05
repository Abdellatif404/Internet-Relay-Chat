#include "User.hpp"

User::User(int fd) : _fd(fd), _hostname("localhost"), _state(UNREGISTERED), 
                     _lastPing(time(NULL)), _connectionTime(time(NULL)), 
                     _authenticated(false), _isOperator(false), _isAway(false) {}

void User::setAway(bool away, const std::string& message)
{
    _isAway = away;
    _awayMessage = away ? message : "";
}

std::string User::getPrefix() const
{
    return _nickname + "!" + _username + "@" + _hostname;
}

bool User::isValidNickname(const std::string& nick)
{
    if (nick.empty() || nick.length() > 30)
        return false;
    
    // Check for invalid characters
    static const std::string invalid_chars = " ,*?!@\r\n\0";
    if (nick.find_first_of(invalid_chars) != std::string::npos)
        return false;
    
    // Check first character restrictions
    char first = nick[0];
    if (first == '$' || first == ':' || first == '#' || first == '&' || first == '+')
        return false;
    
    // Valid characters for nicknames (RFC 2812)
    static const std::string valid_special = "[]\\`_^{|}";
    for (size_t i = 0; i < nick.length(); ++i) {
        char c = nick[i];
        if (!std::isalnum(c) && valid_special.find(c) == std::string::npos && 
            (i > 0 && c != '-'))
            return false;
    }
    return true;
}

bool User::isValidUsername(const std::string& username)
{
    if (username.empty() || username.length() > 20)
        return false;
    
    static const std::string invalid_chars = " @!:";
    return username.find_first_of(invalid_chars) == std::string::npos;
}