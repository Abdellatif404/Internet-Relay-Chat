#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;

class UserCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
private:
    static std::string parseRealname(const std::vector<std::string>& params, size_t startIndex);
};

#endif