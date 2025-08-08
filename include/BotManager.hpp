#ifndef BOTMANAGER_HPP
#define BOTMANAGER_HPP

#include "ft_irc.hpp"
#include "IRCBot.hpp"
#include "MessageParser.hpp"

class UserManager;
class ChannelManager;

class BotManager
{
	private:
		std::map<std::string, IRCBot*> _bots;
		UserManager* _userManager;
		ChannelManager* _channelManager;
		bool _botsEnabled;
		int _nextBotFd;
		
	public:
		BotManager(UserManager* userManager, ChannelManager* channelManager);
		~BotManager();
		IRCBot* createBot(const std::string& nickname, int virtualFd = -1);
		void removeBot(const std::string& nickname);
		IRCBot* getBot(const std::string& nickname);
		std::vector<std::string> getBotList() const;
		void enableBots();
		void disableBots();
		bool areBotsEnabled() const;
		bool isEnabled() const;
		std::string getStatus() const;
		void setupDefaultBot();
		void joinBotToChannel(const std::string& botNick, const std::string& channel);
		void removeBotFromChannel(const std::string& botNick, const std::string& channel);
		void processMessage(const std::string& sender, const std::string& target, const std::string& message);
		void processPrivateMessage(const std::string& sender, const std::string& botNick, const std::string& message);
		void processChannelMessage(const std::string& sender, const std::string& channel, const std::string& message);
		void notifyUserJoin(const std::string& channel, const std::string& user);
		void notifyUserLeave(const std::string& channel, const std::string& user);
		void notifyUserQuit(const std::string& user);
		void notifyChannelCreate(const std::string& channel);
    	void broadcastToAllBots(const std::string& message);
};

#endif
