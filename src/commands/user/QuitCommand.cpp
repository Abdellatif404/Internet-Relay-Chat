#include "QuitCommand.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include <iostream>

bool QuitCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    if (!user) return false;
    
    (void)userManager; 
    
    std::string quitMessage = parseQuitMessage(params);
    
    if (quitMessage.empty()) {
        quitMessage = "Client quit";
    }
    
    if (user->isRegistered()) {
        std::string quitMsg = ":" + user->getPrefix() + " QUIT :" + quitMessage + "\r\n";
        
    }
    
    
    
    
    return true;
}

std::string QuitCommand::parseQuitMessage(const std::vector<std::string>& params) {
    if (params.empty()) {
        return "";
    }
    
    std::string message = params[0];
    
    
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }
    
    
    for (size_t i = 1; i < params.size(); i++) {
        message += " " + params[i];
    }
    
    return message;
}
