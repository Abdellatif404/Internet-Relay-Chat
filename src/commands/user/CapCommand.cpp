#include "CapCommand.hpp"
#include <algorithm>

bool CapCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    if (params.empty()) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_NEEDMOREPARAMS) + " " + nick + " CAP :Not enough parameters\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string subcommand = params[0];
    std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::toupper);
    
    if (subcommand == "LS") {
        handleLS(user, userManager);
    } else if (subcommand == "LIST") {
        handleList(user, userManager);
    } else if (subcommand == "REQ") {
        std::vector<std::string> caps(params.begin() + 1, params.end());
        handleReq(user, caps, userManager);
    } else if (subcommand == "END") {
        handleEnd(user, userManager);
    } else {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_UNKNOWNCOMMAND) + " " + nick + " " + subcommand + " :Unknown command\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    return true;
}

void CapCommand::handleLS(User* user, UserManager* userManager) {
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    // For ft_irc project: minimal capability support for compatibility
    std::string caps = ":localhost CAP " + nick + " LS :\r\n";
    userManager->sendMessage(user, caps);
}

void CapCommand::handleList(User* user, UserManager* userManager) {
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    // Return empty list (no capabilities negotiated for basic ft_irc)
    std::string caps = ":localhost CAP " + nick + " LIST :\r\n";
    userManager->sendMessage(user, caps);
}

void CapCommand::handleReq(User* user, const std::vector<std::string>& caps, UserManager* userManager) {
    std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
    // For ft_irc: reject all capability requests (basic server)
    std::string capList;
    for (size_t i = 0; i < caps.size(); i++) {
        if (i > 0) capList += " ";
        capList += caps[i]; // Don't prefix with - for NAK
    }
    std::string response = ":localhost CAP " + nick + " NAK :" + capList + "\r\n";
    userManager->sendMessage(user, response);
}

void CapCommand::handleEnd(User* user, UserManager* userManager) {
    // End capability negotiation - continue with registration if needed
    // This is important for clients that use CAP negotiation
    if (!user->isRegistered() && !user->getNickname().empty() && !user->getUsername().empty()) {
        userManager->tryCompleteRegistration(user);
    }
}
