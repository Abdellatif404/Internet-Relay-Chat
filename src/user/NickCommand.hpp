#ifndef NICKCOMMAND_HPP
#define NICKCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;

class NickCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif