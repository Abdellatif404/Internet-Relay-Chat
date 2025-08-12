#include "JoinCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "SendQueue.hpp"

JoinCommand::JoinCommand(ChannelManager* channelManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _sendQueue(sendQueue) {
}

JoinCommand::~JoinCommand() {
}

void JoinCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 1) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " JOIN :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string channels = params[0];
    std::string keys = params.size() > 1 ? params[1] : "";

    // Parse comma-separated channels and keys
    std::vector<std::string> channelList;
    std::vector<std::string> keyList;

    std::stringstream channelStream(channels);
    std::string channel;
    while (std::getline(channelStream, channel, ',')) {
        if (!channel.empty()) {
            channelList.push_back(channel);
        }
    }

    if (!keys.empty()) {
        std::stringstream keyStream(keys);
        std::string key;
        while (std::getline(keyStream, key, ',')) {
            keyList.push_back(key);
        }
    }

    // Process each channel
    for (size_t i = 0; i < channelList.size(); ++i) {
        std::string channelName = channelList[i];
        std::string key = i < keyList.size() ? keyList[i] : "";

        if (!isValidChannelName(channelName)) {
            std::string errorMsg = ":server " ERR_NOSUCHCHANNEL " " + user->getNickname() + " " + channelName + " :No such channel\r\n";
            _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            continue;
        }

        Channel* channel = _channelManager->getChannel(channelName);
        
        // Check if user is already on the channel
        if (channel && channel->isMember(user)) {
            continue;
        }

        // Create channel if it doesn't exist
        if (!channel) {
            channel = _channelManager->createChannel(channelName);
        }

        // Check if user can join
        if (!channel->canJoin(user, key)) {
            if (channel->isInviteOnly() && !channel->isInvited(user)) {
                std::string errorMsg = ":server " ERR_INVITEONLYCHAN " " + user->getNickname() + " " + channelName + " :Cannot join channel (+i)\r\n";
                _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            } else if (channel->hasKey() && key != channel->getKey()) {
                std::string errorMsg = ":server " ERR_BADCHANNELKEY " " + user->getNickname() + " " + channelName + " :Cannot join channel (+k)\r\n";
                _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            } else if (channel->hasUserLimit() && static_cast<int>(channel->getMemberCount()) >= channel->getUserLimit()) {
                std::string errorMsg = ":server " ERR_CHANNELISFULL " " + user->getNickname() + " " + channelName + " :Cannot join channel (+l)\r\n";
                _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            }
            continue;
        }

        // Add user to channel
        channel->addUser(user);

        // Send JOIN message to all channel members including the user
        sendJoinMessages(user, channelName);

        // Send NAMES reply
        sendNamesReply(user, channelName);
    }
}

bool JoinCommand::isValidChannelName(const std::string& name) const {
    return _channelManager->isValidChannelName(name);
}

void JoinCommand::sendJoinMessages(User* user, const std::string& channelName) {
    std::string joinMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@hostname JOIN " + channelName + "\r\n";
    
    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        // Send to all members including the user who joined
        channel->broadcastMessage(joinMsg, NULL);
        _sendQueue->enqueueMessage(user->getFd(), joinMsg);

        // Send topic if it exists
        if (!channel->getTopic().empty()) {
            std::string topicMsg = ":server " RPL_TOPIC " " + user->getNickname() + " " + channelName + " :" + channel->getTopic() + "\r\n";
            _sendQueue->enqueueMessage(user->getFd(), topicMsg);
            
            if (!channel->getTopicWho().empty()) {
                std::ostringstream oss;
                oss << ":server " RPL_TOPICWHOTIME " " << user->getNickname() << " " << channelName 
                    << " " << channel->getTopicWho() << " " << channel->getTopicTime() << "\r\n";
                _sendQueue->enqueueMessage(user->getFd(), oss.str());
            }
        } else {
            std::string noTopicMsg = ":server " RPL_NOTOPIC " " + user->getNickname() + " " + channelName + " :No topic is set\r\n";
            _sendQueue->enqueueMessage(user->getFd(), noTopicMsg);
        }
    }
}

void JoinCommand::sendNamesReply(User* user, const std::string& channelName) {
    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        std::string membersList = channel->getMembersList();
        if (!membersList.empty()) {
            std::string namesMsg = ":server " RPL_NAMREPLY " " + user->getNickname() + " = " + channelName + " :" + membersList + "\r\n";
            _sendQueue->enqueueMessage(user->getFd(), namesMsg);
        }
        
        std::string endNamesMsg = ":server " RPL_ENDOFNAMES " " + user->getNickname() + " " + channelName + " :End of /NAMES list\r\n";
        _sendQueue->enqueueMessage(user->getFd(), endNamesMsg);
    }
}
