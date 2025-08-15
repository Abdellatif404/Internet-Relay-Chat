#include "PingCommand.hpp"
#include "User.hpp"
#include "UserManager.hpp"

bool PingCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager) {
    if (!user->isRegistered()) {
        userManager->sendError(user, 451, ":You have not registered");
        return false;
    }
    
    std::string server = params.empty() ? userManager->getServerName() : params[0];
    
    
    std::string pongMsg = ":" + userManager->getServerName() + " PONG " + userManager->getServerName();
    if (!server.empty()) {
        pongMsg += " :" + server;
    }
    pongMsg += "\r\n";
    
    userManager->sendMessage(user, pongMsg);
    return true;
}

bool PongCommand::execute(User* user, const std::vector<std::string>&, UserManager*) {
    user->updateLastPing();
    return true;
}
