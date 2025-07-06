#include "NickCommand.hpp"

bool NickCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    if (!user->isAuthenticated()) {
        std::string error = ":localhost 464 * :Password required\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (params.empty()) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost 431 " + nick + " :No nickname given\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string nickname = params[0];
    
    if (!userManager->registerNickname(user, nickname)) {
        std::string error = ":localhost 433 * " + nickname + " :Nickname is already in use\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::cout << "Nickname set to: " << nickname << " for fd " << user->getFd() << std::endl;
    
    // Try to complete registration if possible
    if (userManager->tryCompleteRegistration(user)) {
        std::cout << "User " << user->getNickname() << " registration completed!" << std::endl;
    }
    
    return true;
}