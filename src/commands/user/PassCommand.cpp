#include "PassCommand.hpp"

bool PassCommand::execute(User* user, const std::vector<std::string>& params, UserManager* userManager)
{
    if (user->isAuthenticated())
	{
        std::string nick = user->getNickname().empty() ? "*" : user->getNickname();
        std::string error = ":localhost " + std::string(ERR_ALREADYREGISTRED) + " " + nick + " :You may not reregister\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    if (params.empty())
	{
        std::string error = ":localhost " + std::string(ERR_NEEDMOREPARAMS) + " * PASS :Not enough parameters\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
    std::string password = params[0];
    if (userManager->authenticateUser(user, password))
	{
        std::cout << "User on fd " << user->getFd() << " authenticated successfully" << std::endl;
        return true;
    }
	else
	{
        std::string error = ":localhost " + std::string(ERR_PASSWDMISMATCH) + " * :Password incorrect\r\n";
        userManager->sendMessage(user, error);
        return false;
    }
}