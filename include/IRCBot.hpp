#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "MessageParser.hpp"

class UserManager;
class ChannelManager;

class IRCBot : public User {
private:
    std::map<std::string, std::string> _responses;
    std::map<std::string, std::string> _helpCommands;
    std::vector<std::string> _channels;
    UserManager* _userManager;
    ChannelManager* _channelManager;
    bool _isActive;
    time_t _startTime;
    void handleHelp(const std::string& target, const std::vector<std::string>& params);
    void handleTime(const std::string& target);
    void handlePing(const std::string& target, const std::vector<std::string>& params);
    void handleEcho(const std::string& target, const std::vector<std::string>& params);
    void handleVersion(const std::string& target);
    void handleUptime(const std::string& target);
    void handleUserInfo(const std::string& target, const std::vector<std::string>& params);
    void handleChannelInfo(const std::string& target, const std::vector<std::string>& params);
    void handleChannels(const std::string& target);
    void handleJoke(const std::string& target);
    void handleQuote(const std::string& target);
    void sendMessage(const std::string& target, const std::string& message);
    void sendNotice(const std::string& target, const std::string& message);
    bool isCommand(const std::string& message);
    std::string extractCommand(const std::string& message);
    std::vector<std::string> extractParams(const std::string& message);
    void initializeResponses();
    void initializeHelpCommands();

public:
    IRCBot(int fd, UserManager* userManager, ChannelManager* channelManager);
    ~IRCBot();
    
    void processMessage(const std::string& sender, const std::string& target, const std::string& message);
    void processPrivateMessage(const std::string& sender, const std::string& message);
    void processChannelMessage(const std::string& sender, const std::string& channel, const std::string& message);
    void joinChannel(const std::string& channel);
    void leaveChannel(const std::string& channel);
    void setActive(bool active);
    bool isActive() const;
    void onUserJoin(const std::string& channel, const std::string& user);
    void onUserLeave(const std::string& channel, const std::string& user);
    void onUserQuit(const std::string& user);
    void addCustomResponse(const std::string& command, const std::string& response);
    void removeCustomResponse(const std::string& command);
    std::string getUptime() const;
    std::vector<std::string> getJoinedChannels() const;
};

#endif
