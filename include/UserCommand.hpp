#ifndef USERCOMMAND_HPP
#define USERCOMMAND_HPP

#include "ft_irc.hpp"

class UserCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);
private:
    static std::string parseRealname(const std::vector<std::string>& params, size_t startIndex);
};

#endif