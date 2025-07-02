#include "UserCommand.hpp"
#include "../../user/User.hpp"
#include "../../user/UserManager.hpp"
#include <iostream>

bool UserCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager)
{
    if (!user->isAuthenticated()) {
        std::string error = ":localhost 464 * :Password required\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (params.size() < 4) {
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost 461 " + nick + " USER :Not enough parameters\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    if (user->isRegistered()) {
        std::string error = ":localhost 462 " + user->getNickname() + " :You may not reregister\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    
    std::string username = params[0];
    // params[1] is mode (ignored for now)
    // params[2] is unused (should be *)
    std::string realname = parseRealname(params, 3);
    
    user->setUsername(username);
    user->setRealname(realname);
    
    std::cout << "User info set for fd " << user->getFd() << ": " << username << " (" << realname << ")" << std::endl;
    
    // Try to complete registration
    if (userManager->tryCompleteRegistration(user)) {
        std::cout << "User " << user->getNickname() << " registration completed!" << std::endl;
    }
    
    return true;
}

std::string UserCommand::parseRealname(const std::vector<std::string>& params, size_t startIndex) {
    if (startIndex >= params.size()) {
        return "";
    }
    
    std::string realname = params[startIndex];
    
    // Remove leading ':' if present
    if (!realname.empty() && realname[0] == ':') {
        realname = realname.substr(1);
    }
    
    // If there are more parameters after the realname, join them with spaces
    for (size_t i = startIndex + 1; i < params.size(); i++) {
        realname += " " + params[i];
    }
    
    return realname;
}