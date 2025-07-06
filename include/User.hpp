#ifndef USER_HPP
#define USER_HPP

#include "ft_irc.hpp"

enum UserState {
    UNREGISTERED,
    PASS_SET,
    NICK_SET,
	USR_REGISTERED
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

    // Setters
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setRealname(const std::string& realname);
    void setHostname(const std::string& hostname);
    void setState(UserState state);
    void setAuthenticated(bool auth);

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
};

#endif