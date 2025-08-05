#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include <string>
#include <vector>

class User;
class UserManager;
class ChannelManager;

class PrivMsgCommand
{
	public:
		static bool execute(User* user, const std::vector<std::string>& params, UserManager* userManager, ChannelManager* channelManager = NULL);
	private:
		static std::string parseMessage(const std::vector<std::string>& params, size_t startIndex);
};

#endif
