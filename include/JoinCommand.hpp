#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class SendQueue;

class JoinCommand {
private:
    ChannelManager* _channelManager;
    SendQueue* _sendQueue;

public:
    JoinCommand(ChannelManager* channelManager, SendQueue* sendQueue);
    ~JoinCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    bool isValidChannelName(const std::string& name) const;
    void sendJoinMessages(User* user, const std::string& channelName);
    void sendNamesReply(User* user, const std::string& channelName);
};

#endif
