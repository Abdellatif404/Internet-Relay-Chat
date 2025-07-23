#include "InviteCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "SendQueue.hpp"

InviteCommand::InviteCommand(ChannelManager* channelManager, UserManager* userManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _userManager(userManager), _sendQueue(sendQueue) {
}

InviteCommand::~InviteCommand() {
}

void InviteCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 2) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " INVITE :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string targetNick = params[0];
    std::string channelName = params[1];

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

    // Check if channel is invite-only and user is not operator
    if (channel->isInviteOnly() && !channel->isOperator(user)) {
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

    if (channel->isMember(targetUser)) {
        std::string errorMsg = ":server " ERR_USERONCHANNEL " " + user->getNickname() + " " + targetNick + " " + channelName + " :is already on channel\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    // Add user to invite list
    channel->inviteUser(targetUser);

    // Send invite notification to target user
    sendInviteNotification(user, targetUser, channelName);

    // Send invite reply to inviting user
    sendInviteReply(user, targetNick, channelName);
    */

    // Placeholder response for now
    std::string errorMsg = ":server " ERR_NOSUCHNICK " " + user->getNickname() + " " + targetNick + " :No such nick/channel\r\n";
    _sendQueue->enqueueMessage(user->getFd(), errorMsg);
}

void InviteCommand::sendInviteNotification(User* inviter, User* invited, const std::string& channelName) {
    std::string inviteMsg = ":" + inviter->getNickname() + "!" + inviter->getUsername() + 
                           "@hostname INVITE " + invited->getNickname() + " " + channelName + "\r\n";
    _sendQueue->enqueueMessage(invited->getFd(), inviteMsg);
}

void InviteCommand::sendInviteReply(User* inviter, const std::string& nickname, const std::string& channelName) {
    std::string replyMsg = ":server " RPL_INVITING " " + inviter->getNickname() + " " + nickname + " " + channelName + "\r\n";
    _sendQueue->enqueueMessage(inviter->getFd(), replyMsg);
}
