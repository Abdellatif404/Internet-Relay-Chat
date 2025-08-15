#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "UserManager.hpp"

class CapCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager);

private:
    static void handleList(User* user, UserManager* userManager);
    static void handleLS(User* user, UserManager* userManager);
    static void handleReq(User* user, const std::vector<std::string>& caps, UserManager* userManager);
    static void handleEnd(User* user, UserManager* userManager);
};

#endif
