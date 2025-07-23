#include "KickCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "SendQueue.hpp"

KickCommand::KickCommand(ChannelManager* channelManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _sendQueue(sendQueue) {
}

KickCommand::~KickCommand() {
}

void KickCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 2) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " KICK :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = params.size() > 2 ? params[2] : user->getNickname();

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

    if (!channel->isOperator(user)) {
        std::string errorMsg = ":server " ERR_CHANOPRIVSNEEDED " " + user->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    // TODO: Find target user by nickname using UserManager
    // For now, this is a placeholder that would need UserManager integration
    /*
    User* targetUser = _userManager->getUserByNickname(targetNick);
    
    if (!targetUser) {
        std::string errorMsg = ":server " ERR_NOSUCHNICK " " + user->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (!channel->isMember(targetUser)) {
        std::string errorMsg = ":server " ERR_USERNOTINCHANNEL " " + user->getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    // Broadcast kick message and remove user
    broadcastKick(user, targetUser, channelName, reason);
    channel->kickUser(targetUser, user, reason);
    */

    // Placeholder response for now
    std::string errorMsg = ":server " ERR_NOSUCHNICK " " + user->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
    _sendQueue->enqueueMessage(user->getFd(), errorMsg);
}

void KickCommand::broadcastKick(User* kicker, User* kicked, 
                               const std::string& channelName, const std::string& reason) {
    std::string kickMsg = ":" + kicker->getNickname() + "!" + kicker->getUsername() + 
                         "@hostname KICK " + channelName + " " + kicked->getNickname() + " :" + reason + "\r\n";
    
    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        // Send to all members including the kicked user
        channel->broadcastMessage(kickMsg, NULL);
        _sendQueue->enqueueMessage(kicked->getFd(), kickMsg);
    }
}
