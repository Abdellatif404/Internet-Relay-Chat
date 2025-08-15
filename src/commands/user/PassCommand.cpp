#include "PassCommand.hpp"

bool PassCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager)
{
    if (user->isRegistered()) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_ALREADYREGISTRED) + " " + nick + " :You may not reregister\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (user->isAuthenticated()) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_ALREADYREGISTRED) + " " + nick + " :Password already accepted\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (params.empty()) {
        std::string error = ":localhost " + std::string(ERR_NEEDMOREPARAMS) + " * PASS :Not enough parameters\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string password = params[0];
    if (userManager->authenticateUser(user, password)) {
        std::string success = ":localhost NOTICE * :Password accepted\r\n";
        userManager->sendMessage(user, success);
        return true;
    } else {
        std::string error = ":localhost " + std::string(ERR_PASSWDMISMATCH) + " * :Password incorrect\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
}