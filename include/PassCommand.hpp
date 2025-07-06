#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include "ft_irc.hpp"

class PassCommand
{
	public:
    	static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif