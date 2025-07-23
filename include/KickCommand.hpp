#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class UserManager;
class SendQueue;

class KickCommand {
private:
    ChannelManager* _channelManager;
    UserManager* _userManager;
    SendQueue* _sendQueue;

public:
    KickCommand(ChannelManager* channelManager, UserManager* userManager, SendQueue* sendQueue);
    ~KickCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void broadcastKick(User* kicker, User* kicked, 
                      const std::string& channelName, const std::string& reason);
};

#endif
