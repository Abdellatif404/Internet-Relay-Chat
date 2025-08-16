#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;
class ChannelManager;
class BotManager;

class PrivMsgCommand {
public:
    static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager, ChannelManager* channelManager = NULL, BotManager* botManager = NULL);
private:
    static std::string parseMessage(const std::vector<std::string>& params, size_t startIndex);
};

#endif
