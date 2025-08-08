#include "BotManager.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"
#include <sstream>

BotManager::BotManager(UserManager* userManager, ChannelManager* channelManager) : _userManager(userManager), _channelManager(channelManager), _botsEnabled(true), _nextBotFd(-1000) {}

BotManager::~BotManager()
{
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        delete it->second;
    }
    _bots.clear();
}

IRCBot* BotManager::createBot(const std::string& nickname, int virtualFd)
{
    if (_bots.find(nickname) != _bots.end())
        return NULL;
    if (virtualFd == -1)
        virtualFd = _nextBotFd--;
    IRCBot* bot = new IRCBot(virtualFd, _userManager, _channelManager);
    bot->setNickname(nickname);
    _bots[nickname] = bot;
    std::cout << GREEN << "Bot '" << nickname << "' created successfully" << RESET << std::endl;
    return bot;
}

void BotManager::removeBot(const std::string& nickname)
{
    std::map<std::string, IRCBot*>::iterator it = _bots.find(nickname);
    if (it != _bots.end())
	{
        IRCBot* bot = it->second;
        std::vector<std::string> channels = bot->getJoinedChannels();
        for (size_t i = 0; i < channels.size(); ++i)
            bot->leaveChannel(channels[i]);
        delete it->second;
        _bots.erase(it);
        
        std::cout << YELLOW << "Bot '" << nickname << "' removed" << RESET << std::endl;
    }
}

IRCBot* BotManager::getBot(const std::string& nickname)
{
    std::map<std::string, IRCBot*>::iterator it = _bots.find(nickname);
    return (it != _bots.end()) ? it->second : NULL;
}

void BotManager::processMessage(const std::string& sender, const std::string& target, const std::string& message)
{
    if (!_botsEnabled)
		return;
    if (target.substr(0, 1) != "#")
        processPrivateMessage(sender, target, message);
    else
        processChannelMessage(sender, target, message);
}

void BotManager::processPrivateMessage(const std::string& sender, const std::string& botNick, const std::string& message)
{
    IRCBot* bot = getBot(botNick);
    if (bot && bot->isActive())
        bot->processMessage(sender, botNick, message);
}

void BotManager::processChannelMessage(const std::string& sender, const std::string& channel, const std::string& message)
{
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        IRCBot* bot = it->second;
        if (bot->isActive())
		{
            std::vector<std::string> botChannels = bot->getJoinedChannels();
            if (std::find(botChannels.begin(), botChannels.end(), channel) != botChannels.end())
			{
                bot->processMessage(sender, channel, message);
            }
        }
    }
}

void BotManager::notifyUserJoin(const std::string& channel, const std::string& user)
{
    if (!_botsEnabled)
		return;
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        IRCBot* bot = it->second;
        if (bot->isActive())
            bot->onUserJoin(channel, user);
    }
}

void BotManager::notifyUserLeave(const std::string& channel, const std::string& user)
{
    if (!_botsEnabled)
		return;
    
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        IRCBot* bot = it->second;
        if (bot->isActive()) {
            bot->onUserLeave(channel, user);
        }
    }
}

void BotManager::notifyUserQuit(const std::string& user) {
    if (!_botsEnabled) return;
    
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it) {
        IRCBot* bot = it->second;
        if (bot->isActive()) {
            bot->onUserQuit(user);
        }
    }
}

void BotManager::notifyChannelCreate(const std::string& channel)
{
    std::cout << BLUE << "Channel " << channel << " created - bots notified" << RESET << std::endl;
}

void BotManager::enableBots()
{
    _botsEnabled = true;
    std::cout << GREEN << "Bots enabled" << RESET << std::endl;
}

void BotManager::disableBots()
{
    _botsEnabled = false;
    std::cout << YELLOW << "Bots disabled" << RESET << std::endl;
}

bool BotManager::areBotsEnabled() const
{
    return _botsEnabled;
}

bool BotManager::isEnabled() const
{
    return _botsEnabled;
}

std::string BotManager::getStatus() const
{
    std::ostringstream status;
    status << "Bot system: " << (areBotsEnabled() ? "enabled" : "disabled");
    status << " | Active bots: " << _bots.size();
    return status.str();
}

std::vector<std::string> BotManager::getBotList() const
{
    std::vector<std::string> botNames;
    for (std::map<std::string, IRCBot*>::const_iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        botNames.push_back(it->first);
    }
    return botNames;
}

void BotManager::joinBotToChannel(const std::string& botNick, const std::string& channel)
{
    IRCBot* bot = getBot(botNick);
    if (bot)
	{
        bot->joinChannel(channel);
    }
}

void BotManager::removeBotFromChannel(const std::string& botNick, const std::string& channel)
{
    IRCBot* bot = getBot(botNick);
    if (bot)
    	bot->leaveChannel(channel);
}

void BotManager::broadcastToAllBots(const std::string& message)
{
    for (std::map<std::string, IRCBot*>::iterator it = _bots.begin(); it != _bots.end(); ++it)
	{
        IRCBot* bot = it->second;
        if (bot->isActive())
            std::cout << "Admin message to bot " << it->first << ": " << message << std::endl;
    }
}

void BotManager::setupDefaultBot()
{
    IRCBot* helpBot = createBot("HelpBot");
    if (helpBot)
	{
        helpBot->addCustomResponse("welcome", "Welcome to our IRC server! I'm here to help you get started.");
        helpBot->addCustomResponse("rules", "Please be respectful and follow our community guidelines.");
        helpBot->addCustomResponse("commands", "Available IRC commands: /join, /part, /msg, /nick, /quit");
        std::cout << GREEN << "Default HelpBot created and configured" << RESET << std::endl;
    }
}
