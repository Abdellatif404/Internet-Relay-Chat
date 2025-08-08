#include "BotCommand.hpp"
#include "MessageParser.hpp"
#include "UserManager.hpp"
#include <sstream>

bool BotCommand::execute(User* user, const std::vector<std::string>& params, BotManager* botManager)
{
    if (!user || !botManager)
        return false;
    if (params.empty())
	{
        handleHelp(user);
        return true;
    }
    std::string subCommand = params[0];
    std::transform(subCommand.begin(), subCommand.end(), subCommand.begin(), ::tolower);
    if (subCommand == "list")
        handleList(user, botManager);
    else if (subCommand == "create" && isOperator(user))
        handleCreate(user, params, botManager);
    else if (subCommand == "remove" && isOperator(user))
        handleRemove(user, params, botManager);
    else if (subCommand == "join" && isOperator(user))
        handleJoin(user, params, botManager);
    else if (subCommand == "part" && isOperator(user))
        handlePart(user, params, botManager);
    else if (subCommand == "enable" && isOperator(user))
        handleEnable(user, botManager);
    else if (subCommand == "disable" && isOperator(user))
        handleDisable(user, botManager);
    else if (subCommand == "status")
        handleStatus(user, botManager);
    else if (subCommand == "help")
        handleHelp(user);
    else {
        if (!isOperator(user))
            sendResponse(user, "Error: You need operator privileges to manage bots");
        else
            sendResponse(user, "Error: Unknown bot command. Use /BOT HELP for available commands");
    }
    return true;
}

void BotCommand::handleList(User* user, BotManager* botManager)
{
    std::vector<std::string> bots = botManager->getBotList();
    if (bots.empty())
	{
        sendResponse(user, "No bots are currently active");
    }
	else
	{
        sendResponse(user, "Active bots:");
        for (size_t i = 0; i < bots.size(); ++i)
		{
            const std::string& botName = bots[i];
            IRCBot* bot = botManager->getBot(botName);
            if (bot) 
			{
                std::ostringstream status;
                status << "- " << botName << " (";
                status << (bot->isActive() ? "active" : "inactive");
                status << ")";
                sendResponse(user, status.str());
            }
        }
    }
}

void BotCommand::handleCreate(User* user, const std::vector<std::string>& params, BotManager* botManager)
{
    if (params.size() < 2)
	{
        sendResponse(user, "Usage: /BOT CREATE <nickname>");
        return;
    }
    std::string botNick = params[1];
    IRCBot* bot = botManager->createBot(botNick);
    if (bot)
        sendResponse(user, "Bot '" + botNick + "' created successfully");
    else
        sendResponse(user, "Error: Could not create bot '" + botNick + "' (nickname may already be in use)");
}

void BotCommand::handleRemove(User* user, const std::vector<std::string>& params, BotManager* botManager)
{
    if (params.size() < 2)
	{
        sendResponse(user, "Usage: /BOT REMOVE <nickname>");
        return;
    }
    std::string botNick = params[1];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
	{
        botManager->removeBot(botNick);
        sendResponse(user, "Bot '" + botNick + "' removed successfully");
    }
	else
	{
        sendResponse(user, "Error: Bot '" + botNick + "' not found");
    }
}

void BotCommand::handleJoin(User* user, const std::vector<std::string>& params, BotManager* botManager)
{
    if (params.size() < 3)
	{
        sendResponse(user, "Usage: /BOT JOIN <nickname> <channel>");
        return;
    }
    std::string botNick = params[1];
    std::string channel = params[2];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
	{
        botManager->joinBotToChannel(botNick, channel);
        sendResponse(user, "Bot '" + botNick + "' joined channel " + channel);
    }
	else
	{
        sendResponse(user, "Error: Bot '" + botNick + "' not found");
    }
}

void BotCommand::handlePart(User* user, const std::vector<std::string>& params, BotManager* botManager)
{
    if (params.size() < 3)
	{
        sendResponse(user, "Usage: /BOT PART <nickname> <channel>");
        return;
    }
    std::string botNick = params[1];
    std::string channel = params[2];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
	{
        botManager->removeBotFromChannel(botNick, channel);
        sendResponse(user, "Bot '" + botNick + "' left channel " + channel);
    }
	else
	{
        sendResponse(user, "Error: Bot '" + botNick + "' not found");
    }
}

void BotCommand::handleEnable(User* user, BotManager* botManager)
{
    botManager->enableBots();
    sendResponse(user, "Bot system enabled");
}

void BotCommand::handleDisable(User* user, BotManager* botManager)
{
    botManager->disableBots();
    sendResponse(user, "Bot system disabled");
}

void BotCommand::handleStatus(User* user, BotManager* botManager)
{
    std::string status = botManager->getStatus();
    sendResponse(user, status);
}

void BotCommand::handleHelp(User* user)
{
    sendResponse(user, "Available BOT commands:");
    sendResponse(user, "/BOT LIST - List all active bots");
    sendResponse(user, "/BOT STATUS - Show bot system status");
    sendResponse(user, "/BOT HELP - Show this help message");
    if (isOperator(user))
	{
        sendResponse(user, "Operator commands:");
        sendResponse(user, "/BOT CREATE <nickname> - Create a new bot");
        sendResponse(user, "/BOT REMOVE <nickname> - Remove a bot");
        sendResponse(user, "/BOT JOIN <nickname> <channel> - Make bot join channel");
        sendResponse(user, "/BOT PART <nickname> <channel> - Make bot leave channel");
        sendResponse(user, "/BOT ENABLE - Enable the bot system");
        sendResponse(user, "/BOT DISABLE - Disable the bot system");
    }
	else
	{
        sendResponse(user, "Note: Operator privileges required for bot management");
    }
}

void BotCommand::sendResponse(User* user, const std::string& message) 
{
    std::string response = ":localhost NOTICE " + user->getNickname() + " :" + message + "\r\n";
    std::cout << "Bot response to " << user->getNickname() << ": " << message << std::endl;
}

bool BotCommand::isOperator(User* user)
{
    return user && user->isRegistered();
}
