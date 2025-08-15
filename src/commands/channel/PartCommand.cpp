#include "PartCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "SendQueue.hpp"

PartCommand::PartCommand(ChannelManager* channelManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _sendQueue(sendQueue) {
}

PartCommand::~PartCommand() {
}

void PartCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 1) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " PART :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string channels = params[0];
    std::string reason = params.size() > 1 ? params[1] : "";

    
    std::vector<std::string> channelList;
    std::stringstream channelStream(channels);
    std::string channel;
    while (std::getline(channelStream, channel, ',')) {
        if (!channel.empty()) {
            channelList.push_back(channel);
        }
    }

    
    for (std::vector<std::string>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
        std::string channelName = *it;
        Channel* channel = _channelManager->getChannel(channelName);

        if (!channel) {
            std::string errorMsg = ":server " ERR_NOSUCHCHANNEL " " + user->getNickname() + " " + channelName + " :No such channel\r\n";
            _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            continue;
        }

        if (!channel->isMember(user)) {
            std::string errorMsg = ":server " ERR_NOTONCHANNEL " " + user->getNickname() + " " + channelName + " :You're not on that channel\r\n";
            _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            continue;
        }

        
        sendPartMessage(user, channelName, reason);

        
        _channelManager->removeUserFromChannel(user, channelName);
    }
}

void PartCommand::sendPartMessage(User* user, const std::string& channelName, const std::string& reason) {
    std::string partMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@hostname PART " + channelName;
    if (!reason.empty()) {
        partMsg += " :" + reason;
    }
    partMsg += "\r\n";

    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        
        channel->broadcastMessage(partMsg, NULL);
        _sendQueue->enqueueMessage(user->getFd(), partMsg);
    }
}
