#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include "ft_irc.hpp"

class User;
class UserManager;

class PingCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
	};

	class PongCommand {
	public:
		static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif
