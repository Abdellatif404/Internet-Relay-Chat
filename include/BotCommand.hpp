#ifndef BOTCOMMAND_HPP
#define BOTCOMMAND_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "BotManager.hpp"

class BotCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, BotManager* botManager);
		
	private:
		static void handleList(User* user, BotManager* botManager);
		static void handleCreate(User* user, const std::vector<std::string>& params, BotManager* botManager);
		static void handleRemove(User* user, const std::vector<std::string>& params, BotManager* botManager);
		static void handleJoin(User* user, const std::vector<std::string>& params, BotManager* botManager);
		static void handlePart(User* user, const std::vector<std::string>& params, BotManager* botManager);
		static void handleEnable(User* user, BotManager* botManager);
		static void handleDisable(User* user, BotManager* botManager);
		static void handleStatus(User* user, BotManager* botManager);
		static void handleHelp(User* user);
		
		static void sendResponse(User* user, const std::string& message);
		static bool isOperator(User* user);
};

#endif
