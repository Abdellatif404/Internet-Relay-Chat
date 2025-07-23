#ifndef MODECOMMAND_HPP
#define MODECOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class SendQueue;

class ModeCommand {
private:
    ChannelManager* _channelManager;
    SendQueue* _sendQueue;

public:
    ModeCommand(ChannelManager* channelManager, SendQueue* sendQueue);
    ~ModeCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void handleChannelMode(User* user, const std::string& channelName, 
                          const std::vector<std::string>& modeParams);
    void handleUserMode(User* user, const std::vector<std::string>& modeParams);
    void parseChannelModes(User* user, const std::string& channelName,
                          const std::string& modes, const std::vector<std::string>& args);
    void applyMode(User* user, const std::string& channelName, 
                   char mode, bool add, const std::string& arg);
    void sendModeReply(User* user, const std::string& channelName, 
                      const std::string& modes, const std::string& args);
};

#endif
