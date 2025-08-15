#include "UserCommand.hpp"

bool UserCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager)
{
    if (!user->isAuthenticated()) {
        std::string error = ":localhost " + std::string(ERR_PASSWDMISMATCH) + " * :Password required\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (params.size() < 4) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_NEEDMOREPARAMS) + " " + nick + " USER :Not enough parameters\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (user->isRegistered()) {
        std::string error = ":localhost " + std::string(ERR_ALREADYREGISTRED) + " " + user->getNickname() + " :You may not reregister\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string username = params[0];
    
    
    std::string realname = parseRealname(params, 3);
    
    
    if (!User::isValidUsername(username)) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_ERRONEUSNICKNAME) + " " + nick + " :Invalid username\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    user->setUsername(username);
    user->setRealname(realname);
    
    if (userManager->tryCompleteRegistration(user)) {
    }
    
    return true;
}

std::string UserCommand::parseRealname(const std::vector<std::string>& params, size_t startIndex) {
    if (startIndex >= params.size()) {
        return "";
    }
    
    std::string realname = params[startIndex];
    
    
    if (!realname.empty() && realname[0] == ':') {
        realname = realname.substr(1);
    }
    
    
    for (size_t i = startIndex + 1; i < params.size(); i++) {
        realname += " " + params[i];
    }
    
    return realname;
}