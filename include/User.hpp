#ifndef USER_HPP
#define USER_HPP

#include "ft_irc.hpp"

enum UserState {
    UNREGISTERED,
    PASS_SET,
    NICK_SET,
    REGISTERED
};

class User {
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    UserState _state;
    bool _authenticated;
    time_t _lastPing;
    time_t _connectionTime;
    bool _isOperator;
    std::string _awayMessage;
    bool _isAway;

public:
    User(int fd);
    ~User();

    
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    UserState getState() const;
    bool isAuthenticated() const;
    bool isRegistered() const;
    time_t getLastPing() const;
    time_t getConnectionTime() const;
    bool isOperator() const;
    const std::string& getAwayMessage() const;
    bool isAway() const;

    
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void setState(UserState state);
    void setAuthenticated(bool auth);
    void setLastPing(time_t ping);
    void setOperator(bool op);
    void setAway(bool away, const std::string& message = "");

    
    std::string getPrefix() const;
    
    
    static bool isValidNickname(const std::string& nick);
    static bool isValidUsername(const std::string& username);
    
    
    void updateLastPing();
    bool needsPing() const;
};

#endif