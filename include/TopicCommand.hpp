#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class ChannelManager;
class SendQueue;

class TopicCommand {
private:
    ChannelManager* _channelManager;
    SendQueue* _sendQueue;

public:
    TopicCommand(ChannelManager* channelManager, SendQueue* sendQueue);
    ~TopicCommand();

    void execute(User* user, const std::vector<std::string>& params);

private:
    void sendTopicReply(User* user, const std::string& channelName, 
                       const std::string& topic);
    void sendNoTopicReply(User* user, const std::string& channelName);
    void broadcastTopicChange(User* user, const std::string& channelName, 
                             const std::string& topic);
};

#endif
