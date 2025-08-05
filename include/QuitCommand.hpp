#ifndef QUITCOMMAND_HPP
#define QUITCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;

class QuitCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
	private:
		static std::string parseQuitMessage(const std::vector<std::string>& params);
};

#endif
