#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include "ft_irc.hpp"
#include "UserManager.hpp"

class NickCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif