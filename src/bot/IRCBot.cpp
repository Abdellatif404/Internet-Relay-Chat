#include "IRCBot.hpp"
#include "UserManager.hpp"
#include "ChannelManager.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>

IRCBot::IRCBot(int fd, UserManager* userManager, ChannelManager* channelManager) : User(fd), _userManager(userManager), _channelManager(channelManager), _isActive(true), _startTime(time(NULL))
{
    setNickname("IRCBot");
    setUsername("ircbot");
    setRealname("IRC Bot Service");
    setHostname("bot.irc.server");
    setState(REGISTERED);
    setAuthenticated(true);
    initializeResponses();
    initializeHelpCommands();
}

IRCBot::~IRCBot() {}

void IRCBot::initializeResponses()
{
    _responses["hello"] = "Hello there! I'm your friendly IRC bot. Type !help to see what I can do!";
    _responses["hi"] = "Hi! How can I help you today?";
    _responses["bot"] = "Yes, I'm a bot! I'm here to help with IRC management and provide useful information.";
    _responses["thanks"] = "You're welcome! Happy to help!";
    _responses["thank you"] = "You're very welcome!";
}

void IRCBot::initializeHelpCommands()
{
    _helpCommands["help"] = "Display this help message";
    _helpCommands["time"] = "Show current server time";
    _helpCommands["ping"] = "Ping the bot (usage: !ping [message])";
    _helpCommands["echo"] = "Echo back your message (usage: !echo <message>)";
    _helpCommands["version"] = "Show bot version information";
    _helpCommands["uptime"] = "Show bot uptime";
    _helpCommands["userinfo"] = "Get information about a user (usage: !userinfo <nickname>)";
    _helpCommands["channelinfo"] = "Get information about current channel";
    _helpCommands["joke"] = "Tell a random joke";
    _helpCommands["quote"] = "Share an inspirational quote";
}

void IRCBot::processMessage(const std::string& sender, const std::string& target, const std::string& message)
{
    if (!_isActive)
		return;
    if (target == getNickname())
        processPrivateMessage(sender, message);
    else if (target.substr(0, 1) == "#")
        processChannelMessage(sender, target, message);
}

void IRCBot::processPrivateMessage(const std::string& sender, const std::string& message)
{
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    for (std::map<std::string, std::string>::const_iterator it = _responses.begin(); it != _responses.end(); ++it)
	{
        if (lowerMessage.find(it->first) != std::string::npos)
		{
            sendMessage(sender, it->second);
            return;
        }
    }
    if (isCommand(message))
	{
        std::string command = extractCommand(message);
        std::vector<std::string> params = extractParams(message);
        if (command == "help")
            handleHelp(sender, params);
        else if (command == "time")
            handleTime(sender);
        else if (command == "ping")
            handlePing(sender, params);
        else if (command == "echo")
            handleEcho(sender, params);
        else if (command == "version")
            handleVersion(sender);
        else if (command == "uptime")
            handleUptime(sender);
        else if (command == "userinfo")
            handleUserInfo(sender, params);
        else if (command == "joke")
            handleJoke(sender);
        else if (command == "quote")
            handleQuote(sender);
        else
            sendMessage(sender, "Unknown command. Type !help for available commands.");
    }
	else
	{
        sendMessage(sender, "Hello! I'm an IRC bot. Type !help to see what I can do for you.");
    }
}

void IRCBot::processChannelMessage(const std::string& sender, const std::string& channel, const std::string& message)
{
    (void)channel; // Suppress unused parameter warning
    std::string lowerMessage = message;
    std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
    if (isCommand(message))
	{
        std::string command = extractCommand(message);
        std::vector<std::string> params = extractParams(message);
        // For channel commands, respond to the sender instead of the channel
        if (command == "help")
            handleHelp(sender, params);
        else if (command == "time")
            handleTime(sender);
        else if (command == "ping")
            handlePing(sender, params);
        else if (command == "echo")
            handleEcho(sender, params);
        else if (command == "version")
            handleVersion(sender);
        else if (command == "uptime")
            handleUptime(sender);
        else if (command == "userinfo")
            handleUserInfo(sender, params);
        else if (command == "channelinfo")
            handleChannelInfo(sender, params);
        else if (command == "joke")
            handleJoke(sender);
        else if (command == "quote")
            handleQuote(sender);
    }
    else if (lowerMessage.find(getNickname()) != std::string::npos ||  lowerMessage.find("bot") != std::string::npos)
	{
        for (std::map<std::string, std::string>::const_iterator it = _responses.begin(); it != _responses.end(); ++it)
		{
            if (lowerMessage.find(it->first) != std::string::npos)
			{
                sendMessage(sender, sender + ": " + it->second);
                return;
            }
        }
        sendMessage(sender, sender + ": Hello! Type !help to see what I can do.");
    }
}

void IRCBot::handleHelp(const std::string& target, const std::vector<std::string>& params)
{
    if (params.empty())
	{
        sendMessage(target, "Available commands:");
        for (std::map<std::string, std::string>::const_iterator it = _helpCommands.begin(); it != _helpCommands.end(); ++it)
		{
            sendMessage(target, "!" + it->first + " - " + it->second);
        }
    }
	else
	{
        std::string command = params[0];
        std::map<std::string, std::string>::const_iterator it = _helpCommands.find(command);
        if (it != _helpCommands.end())
		{
            sendMessage(target, "!" + command + " - " + it->second);
        }
		else
		{
            sendMessage(target, "Unknown command: " + command);
        }
    }
}

void IRCBot::handleTime(const std::string& target)
{
    time_t now = time(NULL);
    std::string timeStr = ctime(&now);
    timeStr.erase(timeStr.length() - 1);
    sendMessage(target, "Current server time: " + timeStr);
}

void IRCBot::handlePing(const std::string& target, const std::vector<std::string>& params)
{
    if (params.empty())
	{
        sendMessage(target, "Pong!");
    }
	else
	{
        std::string message;
        for (size_t i = 0; i < params.size(); ++i)
		{
            if (i > 0)
				message += " ";
            message += params[i];
        }
        sendMessage(target, "Pong: " + message);
    }
}

void IRCBot::handleEcho(const std::string& target, const std::vector<std::string>& params)
{
    if (params.empty())
	{
        sendMessage(target, "Usage: !echo <message>");
        return;
    }
    
    std::string message;
    for (size_t i = 0; i < params.size(); ++i)
	{
        if (i > 0)
			message += " ";
        message += params[i];
    }
    sendMessage(target, "Echo: " + message);
}

void IRCBot::handleVersion(const std::string& target)
{
    sendMessage(target, "IRC Bot v1.0 - Built for ft_irc project");
}

void IRCBot::handleUptime(const std::string& target)
{
    sendMessage(target, "Bot uptime: " + getUptime());
}

void IRCBot::handleUserInfo(const std::string& target, const std::vector<std::string>& params)
{
    if (params.empty())
	{
        sendMessage(target, "Usage: !userinfo <nickname>");
        return;
    }
    
    if (_userManager)
	{
        User* user = _userManager->getUserByNickname(params[0]);
        if (user) {
            std::stringstream info;
            info << "User " << user->getNickname() << " (" << user->getUsername() << "@" << user->getHostname() << ")";
            if (user->isAway())
			{
                info << " - Away: " << user->getAwayMessage();
            }
            if (user->isOperator())
			{
                info << " - IRC Operator";
            }
            sendMessage(target, info.str());
        }
		else
		{
            sendMessage(target, "User " + params[0] + " not found.");
        }
    }
	else
	{
        sendMessage(target, "User information not available (UserManager not initialized).");
    }
}

void IRCBot::handleChannelInfo(const std::string& target, const std::vector<std::string>& )
{
    if (target.substr(0, 1) != "#")
	{
        sendMessage(target, "This command can only be used in channels.");
        return;
    }
    sendMessage(target, "Channel: " + target + " - Bot monitoring active");
}

void IRCBot::handleJoke(const std::string& target)
{
    std::vector<std::string> jokes;
    jokes.push_back("Why do programmers prefer dark mode? Because light attracts bugs!");
    jokes.push_back("How many programmers does it take to change a light bulb? None, that's a hardware problem!");
    jokes.push_back("Why do Java developers wear glasses? Because they can't C#!");
    jokes.push_back("What's a programmer's favorite hangout place? Foo Bar!");
    jokes.push_back("Why did the programmer quit his job? He didn't get arrays!");
    jokes.push_back("What do you call a programmer from Finland? Nerdic!");
    jokes.push_back("Why do programmers hate nature? It has too many bugs!");
    int randomIndex = rand() % jokes.size();
    sendMessage(target, jokes[randomIndex]);
}

void IRCBot::handleQuote(const std::string& target)
{
    std::vector<std::string> quotes;
    quotes.push_back("\"Code is like humor. When you have to explain it, it's bad.\" - Cory House");
    quotes.push_back("\"First, solve the problem. Then, write the code.\" - John Johnson");
    quotes.push_back("\"Programming is not about typing, it's about thinking.\" - Rich Hickey");
    quotes.push_back("\"The best error message is the one that never shows up.\" - Thomas Fuchs");
    quotes.push_back("\"Clean code always looks like it was written by someone who cares.\" - Robert C. Martin");
    quotes.push_back("\"Programs must be written for people to read, and only incidentally for machines to execute.\" - Harold Abelson");
    quotes.push_back("\"Any fool can write code that a computer can understand. Good programmers write code that humans can understand.\" - Martin Fowler");
    
    int randomIndex = rand() % quotes.size();
    sendMessage(target, quotes[randomIndex]);
}

void IRCBot::sendMessage(const std::string& target, const std::string& message)
{
    if (!_userManager)
        return;
        
    User* targetUser = _userManager->getUserByNickname(target);
    if (targetUser)
    {
        std::string response = ":" + getPrefix() + " PRIVMSG " + target + " :" + message + "\r\n";
        _userManager->sendMessage(targetUser, response);
        std::cout << "Bot message to " << target << ": " << message << std::endl;
    }
    else if (target[0] == '#')
    {
        std::cout << "Bot message to channel " << target << ": " << message << std::endl;
    }
    else
    {
        std::cout << "Bot: Target user " << target << " not found" << std::endl;
    }
}

void IRCBot::sendNotice(const std::string& target, const std::string& message)
{
    if (!_userManager)
        return;
        
    User* targetUser = _userManager->getUserByNickname(target);
    if (targetUser)
    {
        std::string response = ":" + getPrefix() + " NOTICE " + target + " :" + message + "\r\n";
        _userManager->sendMessage(targetUser, response);
        std::cout << "Bot notice to " << target << ": " << message << std::endl;
    }
    else
    {
        std::cout << "Bot: Target user " << target << " not found for notice" << std::endl;
    }
}

bool IRCBot::isCommand(const std::string& message)
{
    return !message.empty() && message[0] == '!';
}

std::string IRCBot::extractCommand(const std::string& message)
{
    if (!isCommand(message))
		return "";
    size_t spacePos = message.find(' ');
    if (spacePos == std::string::npos)
	{
        return message.substr(1);
    }
	else
	{
        return message.substr(1, spacePos - 1);
    }
}

std::vector<std::string> IRCBot::extractParams(const std::string& message)
{
    std::vector<std::string> params;
    size_t spacePos = message.find(' ');
    
    if (spacePos != std::string::npos)
	{
        std::string paramStr = message.substr(spacePos + 1);
        std::istringstream iss(paramStr);
        std::string param;
        while (iss >> param)
            params.push_back(param);
    }
    return params;
}

void IRCBot::joinChannel(const std::string& channel)
{
    if (std::find(_channels.begin(), _channels.end(), channel) == _channels.end()) {
        _channels.push_back(channel);
        IRCMessage msg;
        msg.prefix = getPrefix();
        msg.command = "JOIN";
        msg.params.push_back(channel);
        std::cout << "Bot joining: " << MessageParser::serialize(msg) << std::endl;
    }
}

void IRCBot::leaveChannel(const std::string& channel)
{
    std::vector<std::string>::iterator it = std::find(_channels.begin(), _channels.end(), channel);
    if (it != _channels.end())
	{
        _channels.erase(it);
        IRCMessage msg;
        msg.prefix = getPrefix();
        msg.command = "PART";
        msg.params.push_back(channel);
        msg.trailing = "Bot leaving channel";
        std::cout << "Bot leaving: " << MessageParser::serialize(msg) << std::endl;
    }
}

void IRCBot::setActive(bool active)
{
    _isActive = active;
}

bool IRCBot::isActive() const
{
    return _isActive;
}

void IRCBot::onUserJoin(const std::string& channel, const std::string& user)
{
    if (_isActive && std::find(_channels.begin(), _channels.end(), channel) != _channels.end())
	{
        sendMessage(channel, "Welcome to " + channel + ", " + user + "! Type !help if you need assistance.");
    }
}

void IRCBot::onUserLeave(const std::string& channel, const std::string& user)
{
    if (_isActive && std::find(_channels.begin(), _channels.end(), channel) != _channels.end())
	{
        sendMessage(channel, "Goodbye, " + user + "!");
    }
}

void IRCBot::onUserQuit(const std::string&)
{
}

void IRCBot::addCustomResponse(const std::string& command, const std::string& response)
{
    _responses[command] = response;
}

void IRCBot::removeCustomResponse(const std::string& command)
{
    _responses.erase(command);
}

std::string IRCBot::getUptime() const
{
    time_t now = time(NULL);
    time_t uptime = now - _startTime;
    int days = uptime / 86400;
    int hours = (uptime % 86400) / 3600;
    int minutes = (uptime % 3600) / 60;
    int seconds = uptime % 60;
    std::stringstream ss;
    if (days > 0)
	{
        ss << days << " days, ";
    }
    ss << hours << " hours, " << minutes << " minutes, " << seconds << " seconds";
    return ss.str();
}

std::vector<std::string> IRCBot::getJoinedChannels() const
{
    return _channels;
}
