#include "TopicCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "SendQueue.hpp"

TopicCommand::TopicCommand(ChannelManager* channelManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _sendQueue(sendQueue) {
}

TopicCommand::~TopicCommand() {
}

void TopicCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 1) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " TOPIC :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string channelName = params[0];
    Channel* channel = _channelManager->getChannel(channelName);

    if (!channel) {
        std::string errorMsg = ":server " ERR_NOSUCHCHANNEL " " + user->getNickname() + " " + channelName + " :No such channel\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (!channel->isMember(user)) {
        std::string errorMsg = ":server " ERR_NOTONCHANNEL " " + user->getNickname() + " " + channelName + " :You're not on that channel\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    
    if (params.size() == 1) {
        if (channel->getTopic().empty()) {
            sendNoTopicReply(user, channelName);
        } else {
            sendTopicReply(user, channelName, channel->getTopic());
            
            if (!channel->getTopicWho().empty()) {
                std::ostringstream oss;
                oss << ":server " RPL_TOPICWHOTIME " " << user->getNickname() << " " << channelName 
                    << " " << channel->getTopicWho() << " " << channel->getTopicTime() << "\r\n";
                _sendQueue->enqueueMessage(user->getFd(), oss.str());
            }
        }
        return;
    }

    
    std::string newTopic = params[1];

    
    if (channel->isTopicRestricted() && !channel->isOperator(user)) {
        std::string errorMsg = ":server " ERR_CHANOPRIVSNEEDED " " + user->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    
    channel->setTopic(newTopic, user);

    
    broadcastTopicChange(user, channelName, newTopic);
}

void TopicCommand::sendTopicReply(User* user, const std::string& channelName, const std::string& topic) {
    std::string topicMsg = ":server " RPL_TOPIC " " + user->getNickname() + " " + channelName + " :" + topic + "\r\n";
    _sendQueue->enqueueMessage(user->getFd(), topicMsg);
}

void TopicCommand::sendNoTopicReply(User* user, const std::string& channelName) {
    std::string noTopicMsg = ":server " RPL_NOTOPIC " " + user->getNickname() + " " + channelName + " :No topic is set\r\n";
    _sendQueue->enqueueMessage(user->getFd(), noTopicMsg);
}

void TopicCommand::broadcastTopicChange(User* user, const std::string& channelName, const std::string& topic) {
    std::string topicMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@hostname TOPIC " + channelName + " :" + topic + "\r\n";
    
    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        
        channel->broadcastMessage(topicMsg, NULL);
        _sendQueue->enqueueMessage(user->getFd(), topicMsg);
    }
}
