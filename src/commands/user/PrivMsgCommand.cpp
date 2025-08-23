#include "PrivMsgCommand.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "BotManager.hpp"

bool PrivMsgCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager, ChannelManager* channelManager, BotManager* botManager) {
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
        // Channel message
        if (channelManager) {
            Channel* channel = channelManager->getChannel(target);
            if (channel && channel->isMember(user)) {
                // Format message according to IRC protocol
                std::string prefix = user->getPrefix();
                std::string formattedMsg = ":" + prefix + " PRIVMSG " + target + " :" + message + "\r\n";
                channel->broadcastMessage(formattedMsg, user);
            } else {
                userManager->sendError(user, 403, target + " :No such channel");
            }
        } else {
            userManager->sendError(user, 403, target + " :No such channel");
        }
    } else {
        // Private message
        // First check if target is a bot
        if (botManager) {
            IRCBot* bot = botManager->getBot(target);
            if (bot) {
                // Send message to bot for processing
                botManager->processPrivateMessage(user->getNickname(), target, message);
                return true;
            }
        }
        // If not a bot, send as regular private message
        userManager->sendPrivateMessage(user, target, message);
    }
    
    return true;
}

std::string PrivMsgCommand::parseMessage(const std::vector<std::string>& params, size_t startIndex) {
    if (startIndex >= params.size()) {
        return "";
    }
    
    std::string message = params[startIndex];
    
    // Remove leading ':' if present
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }
    
    // Join remaining parameters with spaces
    for (size_t i = startIndex + 1; i < params.size(); i++) {
        message += " " + params[i];
    }
    
    return message;
}
