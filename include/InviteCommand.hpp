#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class UserManager;
class SendQueue;

class InviteCommand {
private:
    ChannelManager* _channelManager;
    UserManager* _userManager;
    SendQueue* _sendQueue;

public:
    InviteCommand(ChannelManager* channelManager, UserManager* userManager, SendQueue* sendQueue);
    ~InviteCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void sendInviteNotification(User* inviter, User* invited, const std::string& channelName);
    void sendInviteReply(User* inviter, const std::string& nickname, const std::string& channelName);
};

#endif
