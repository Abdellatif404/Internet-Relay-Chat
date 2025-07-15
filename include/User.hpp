#ifndef USER_HPP
#define USER_HPP

#include "ft_irc.hpp"
#include <ctime>

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
    std::vector<std::string> _channels;
    std::string _buffer;
    time_t _lastPing;
    time_t _connectionTime;
    bool _isOperator;
    std::string _awayMessage;
    bool _isAway;

public:
    User(int fd);
    ~User();

    // Getters
    int getFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getRealname() const;
    const std::string& getHostname() const;
    UserState getState() const;
    bool isAuthenticated() const;
    bool isRegistered() const;
    const std::vector<std::string>& getChannels() const;
    const std::string& getBuffer() const;
    time_t getLastPing() const;
    time_t getConnectionTime() const;
    bool isOperator() const;
    const std::string& getAwayMessage() const;
    bool isAway() const;

    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void setState(UserState state);
    void setAuthenticated(bool auth);
    void setLastPing(time_t ping);
    void setOperator(bool op);
    void setAway(bool away, const std::string& message = "");

    // Buffer management
    void appendToBuffer(const std::string& data);
    void clearBuffer();
    bool hasCompleteMessage() const;
    std::string extractMessage();

    // Channel management
    void joinChannel(const std::string& channel);
    void leaveChannel(const std::string& channel);
    bool isInChannel(const std::string& channel) const;

    // User prefix for messages (nick!user@host)
    std::string getPrefix() const;
    
    // Validation methods
    static bool isValidNickname(const std::string& nick);
    static bool isValidUsername(const std::string& username);
    
    // Utility
    void updateLastPing();
    bool needsPing() const;
};

#endif