#ifndef BOTCOMMAND_HPP
#define BOTCOMMAND_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "BotManager.hpp"

class BotCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager);
		
	private:
		static void handleList(User* user, BotManager* botManager, UserManager* userManager);
		static void handleCreate(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager);
		static void handleRemove(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager);
		static void handleJoin(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager);
		static void handlePart(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager);
		static void handleEnable(User* user, BotManager* botManager, UserManager* userManager);
		static void handleDisable(User* user, BotManager* botManager, UserManager* userManager);
		static void handleStatus(User* user, BotManager* botManager, UserManager* userManager);
		static void handleHelp(User* user, UserManager* userManager);
		
		static void sendResponse(User* user, const std::string& message, UserManager* userManager);
		static bool isOperator(User* user);
};

#endif
