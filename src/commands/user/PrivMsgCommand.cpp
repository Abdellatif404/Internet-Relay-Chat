#include "PrivMsgCommand.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"
#include "Channel.hpp"
#include <iostream>

bool PrivMsgCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager, ChannelManager* channelManager) {
    if (!user->isRegistered()) {
        userManager->sendError(user, 451, ":You have not registered");
        return false;
    }
    
    if (params.size() < 2) {
        userManager->sendError(user, 411, ":No recipient given (PRIVMSG)");
        return false;
    }
    
    std::string target = params[0];
    std::string message = parseMessage(params, 1);
    
    if (message.empty()) {
        userManager->sendError(user, 412, ":No text to send");
        return false;
    }
    
    if (target[0] == '#' || target[0] == '&') {
        
        if (channelManager) {
            Channel* channel = channelManager->getChannel(target);
            if (!channel) {
                userManager->sendError(user, 403, target + " :No such channel");
                return false;
            }
            
            if (!channel->isMember(user)) {
                userManager->sendError(user, 404, target + " :Cannot send to channel");
                return false;
            }
            
            std::string privMsg = ":" + user->getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
            channel->broadcastMessage(privMsg, user);
        } else {
            userManager->sendError(user, 403, target + " :No such channel");
            return false;
        }
    } else {
        
        userManager->sendPrivateMessage(user, target, message);
    }
    
    return true;
}

std::string PrivMsgCommand::parseMessage(const std::vector<std::string>& params, size_t startIndex) {
    if (startIndex >= params.size()) {
        return "";
    }
    
    std::string message = params[startIndex];
    
    
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }
    
    
    for (size_t i = startIndex + 1; i < params.size(); i++) {
        message += " " + params[i];
    }
    
    return message;
}
