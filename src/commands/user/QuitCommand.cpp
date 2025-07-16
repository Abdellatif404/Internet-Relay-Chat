#include "QuitCommand.hpp"
#include "User.hpp"
#include "UserManager.hpp"
#include <iostream>

bool QuitCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    std::string quitMessage = parseQuitMessage(params);
    
    if (quitMessage.empty()) {
        quitMessage = "Client quit";
    }
    
    std::cout << "User " << user->getNickname() << " quitting: " << quitMessage << std::endl;
    
    // Handle user quit (this will send QUIT message to channels and clean up)
    userManager->handleUserQuit(user, quitMessage);
    
    return true; // Return true even though user will be disconnected
}

std::string QuitCommand::parseQuitMessage(const std::vector<std::string>& params) {
    if (params.empty()) {
        return "";
    }
    
    std::string message = params[0];
    
    // Remove leading ':' if present
    if (!message.empty() && message[0] == ':') {
        message = message.substr(1);
    }
    
    // Join all parameters with spaces
    for (size_t i = 1; i < params.size(); i++) {
        message += " " + params[i];
    }
    
    return message;
}
