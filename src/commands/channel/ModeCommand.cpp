#include "ModeCommand.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include "SendQueue.hpp"

ModeCommand::ModeCommand(ChannelManager* channelManager, SendQueue* sendQueue)
    : _channelManager(channelManager), _sendQueue(sendQueue) {
}

ModeCommand::~ModeCommand() {
}

void ModeCommand::execute(User* user, const std::vector<std::string>& params) {
    if (!user->isRegistered()) {
        std::string errorMsg = ":server " ERR_NOTREGISTERED " " + user->getNickname() + " :You have not registered\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    if (params.size() < 1) {
        std::string errorMsg = ":server " ERR_NEEDMOREPARAMS " " + user->getNickname() + " MODE :Not enough parameters\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string target = params[0];

    // Check if target is a channel (starts with # or &)
    if (target[0] == '#' || target[0] == '&') {
        handleChannelMode(user, target, params);
    } else {
        handleUserMode(user, params);
    }
}

void ModeCommand::handleChannelMode(User* user, const std::string& channelName, 
                                   const std::vector<std::string>& modeParams) {
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

    // If no modes specified, return current channel modes
    if (modeParams.size() == 1) {
        std::string modeString = channel->getModeString();
        std::string modeArgs = "";
        
        if (channel->hasKey()) {
            modeArgs += " " + channel->getKey();
        }
        if (channel->hasUserLimit()) {
            std::ostringstream oss;
            oss << " " << channel->getUserLimit();
            modeArgs += oss.str();
        }
        
        std::string modeMsg = ":server " RPL_CHANNELMODEIS " " + user->getNickname() + " " + channelName + " " + modeString + modeArgs + "\r\n";
        _sendQueue->enqueueMessage(user->getFd(), modeMsg);
        return;
    }

    // Check if user is operator for mode changes
    if (!channel->isOperator(user)) {
        std::string errorMsg = ":server " ERR_CHANOPRIVSNEEDED " " + user->getNickname() + " " + channelName + " :You're not channel operator\r\n";
        _sendQueue->enqueueMessage(user->getFd(), errorMsg);
        return;
    }

    std::string modes = modeParams[1];
    std::vector<std::string> args(modeParams.begin() + 2, modeParams.end());
    
    parseChannelModes(user, channelName, modes, args);
}

void ModeCommand::handleUserMode(User* user, const std::vector<std::string>& modeParams) {
    (void)modeParams; // Suppress unused parameter warning
    // For now, just return current user modes (empty)
    std::string modeMsg = ":server " RPL_UMODEIS " " + user->getNickname() + " +\r\n";
    _sendQueue->enqueueMessage(user->getFd(), modeMsg);
}

void ModeCommand::parseChannelModes(User* user, const std::string& channelName,
                                   const std::string& modes, const std::vector<std::string>& args) {
    bool add = true;
    size_t argIndex = 0;
    std::string appliedModes = "";
    std::string appliedArgs = "";

    for (size_t i = 0; i < modes.length(); ++i) {
        char c = modes[i];
        
        if (c == '+') {
            add = true;
            continue;
        } else if (c == '-') {
            add = false;
            continue;
        }

        std::string arg = "";
        if ((c == 'o' || c == 'k' || c == 'l') && argIndex < args.size()) {
            arg = args[argIndex++];
        }

        try {
            applyMode(user, channelName, c, add, arg);
            
            // Build mode string for broadcast
            if (appliedModes.empty() || appliedModes[appliedModes.length() - 1] != (add ? '+' : '-')) {
                appliedModes += (add ? '+' : '-');
            }
            appliedModes += c;
            
            if (!arg.empty()) {
                if (!appliedArgs.empty()) appliedArgs += " ";
                appliedArgs += arg;
            }
        } catch (const std::exception& e) {
            // Mode application failed, continue with next mode
            continue;
        }
    }

    if (!appliedModes.empty()) {
        sendModeReply(user, channelName, appliedModes, appliedArgs);
    }
}

void ModeCommand::applyMode(User* user, const std::string& channelName, 
                           char mode, bool add, const std::string& arg) {
    Channel* channel = _channelManager->getChannel(channelName);
    if (!channel) return;

    switch (mode) {
        case 'i': // invite-only
            channel->setInviteOnly(add);
            break;
        case 't': // topic restricted
            channel->setTopicRestricted(add);
            break;
        case 'm': // moderated
            channel->setModerated(add);
            break;
        case 'n': // no external messages
            channel->setNoExternalMessages(add);
            break;
        case 's': // secret
            channel->setSecret(add);
            break;
        case 'p': // private
            channel->setPrivate(add);
            break;
        case 'k': // key
            if (add && !arg.empty()) {
                channel->setKey(arg);
            } else if (!add) {
                channel->removeKey();
            }
            break;
        case 'l': // user limit
            if (add && !arg.empty()) {
                int limit = std::atoi(arg.c_str());
                if (limit > 0) {
                    channel->setUserLimit(limit);
                }
            } else if (!add) {
                channel->removeUserLimit();
            }
            break;
        case 'o': // operator
            if (!arg.empty()) {
                // Here you would need UserManager to find user by nickname
                // For now, this is a placeholder
                if (add) {
                    // channel->addOperator(targetUser);
                } else {
                    // channel->removeOperator(targetUser);
                }
            }
            break;
        default:
            std::string errorMsg = ":server " ERR_UNKNOWNMODE " " + user->getNickname() + " " + mode + " :is unknown mode char to me\r\n";
            _sendQueue->enqueueMessage(user->getFd(), errorMsg);
            throw std::runtime_error("Unknown mode");
    }
}

void ModeCommand::sendModeReply(User* user, const std::string& channelName, 
                               const std::string& modes, const std::string& args) {
    std::string modeMsg = ":" + user->getNickname() + "!" + user->getUsername() + "@hostname MODE " + channelName + " " + modes;
    if (!args.empty()) {
        modeMsg += " " + args;
    }
    modeMsg += "\r\n";

    Channel* channel = _channelManager->getChannel(channelName);
    if (channel) {
        // Send to all members including the user who changed the mode
        channel->broadcastMessage(modeMsg, NULL);
        _sendQueue->enqueueMessage(user->getFd(), modeMsg);
    }
}
