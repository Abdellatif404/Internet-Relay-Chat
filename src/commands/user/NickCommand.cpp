#include "NickCommand.hpp"

bool NickCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    if (!user->isAuthenticated()) {
        std::string error = ":localhost " + std::string(ERR_PASSWDMISMATCH) + " * :Password required\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (params.empty()) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_NONICKNAMEGIVEN) + " " + nick + " :No nickname given\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string nickname = params[0];
    
    
    if (!User::isValidNickname(nickname)) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_ERRONEUSNICKNAME) + " " + nick + " " + nickname + " :Erroneous nickname\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    
    if (user->isRegistered()) {
        if (!userManager->changeNickname(user, nickname)) {
            std::string error = ":localhost " + std::string(ERR_NICKNAMEINUSE) + " " + user->getNickname() + " " + nickname + " :Nickname is already in use\r\n";
            userManager->sendMessage(user, error);
            return false;
        }
    } else {
        
        if (!userManager->registerNickname(user, nickname)) {
            std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
            std::string error = ":localhost " + std::string(ERR_NICKNAMEINUSE) + " " + nick + " " + nickname + " :Nickname is already in use\r\n";
            userManager->sendMessage(user, error);
            return false;
        }
        
        std::string success = ":localhost NOTICE " + nickname + " :Nickname set successfully\r\n";
        userManager->sendMessage(user, success);
        
        if (userManager->tryCompleteRegistration(user)) {
        }
    }
    
    return true;
}