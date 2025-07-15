#ifndef PINGCOMMAND_HPP
#define PINGCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;

class PingCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

class PongCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
};

#endif
