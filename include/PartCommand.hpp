#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class SendQueue;

class PartCommand {
private:
    ChannelManager* _channelManager;
    SendQueue* _sendQueue;

public:
    PartCommand(ChannelManager* channelManager, SendQueue* sendQueue);
    ~PartCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void sendPartMessage(User* user, const std::string& channelName, const std::string& reason);
};

#endif
