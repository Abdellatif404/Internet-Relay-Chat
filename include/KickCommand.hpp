#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class SendQueue;

class KickCommand {
private:
    ChannelManager* _channelManager;
    SendQueue* _sendQueue;

public:
    KickCommand(ChannelManager* channelManager, SendQueue* sendQueue);
    ~KickCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void broadcastKick(User* kicker, User* kicked, 
                      const std::string& channelName, const std::string& reason);
};

#endif
