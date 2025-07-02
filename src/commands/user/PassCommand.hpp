#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;

class PassCommand
{
	public:
    	static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif