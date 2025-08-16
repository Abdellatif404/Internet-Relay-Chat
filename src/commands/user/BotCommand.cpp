#include "BotCommand.hpp"
#include "MessageParser.hpp"
#include "UserManager.hpp"
#include <sstream>

bool BotCommand::execute(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager)
{
    if (params.empty())
    {
        sendResponse(user, "Usage: /BOT <subcommand>. Use /BOT HELP for available commands", userManager);
        return true;
    }

    std::string subCommand = params[0];
    
    // Convert to uppercase for case-insensitive comparison
    for (size_t i = 0; i < subCommand.length(); ++i)
    {
        subCommand[i] = std::toupper(subCommand[i]);
    }

    if (subCommand == "HELP")
    {
        handleHelp(user, userManager);
    }
    else if (subCommand == "LIST")
    {
        handleList(user, botManager, userManager);
    }
    else if (subCommand == "CREATE" && isOperator(user))
    {
        handleCreate(user, params, botManager, userManager);
    }
    else if (subCommand == "REMOVE" && isOperator(user))
    {
        handleRemove(user, params, botManager, userManager);
    }
    else if (subCommand == "JOIN" && isOperator(user))
    {
        handleJoin(user, params, botManager, userManager);
    }
    else if (subCommand == "PART" && isOperator(user))
    {
        handlePart(user, params, botManager, userManager);
    }
    else if (subCommand == "ENABLE" && isOperator(user))
    {
        handleEnable(user, botManager, userManager);
    }
    else if (subCommand == "DISABLE" && isOperator(user))
    {
        handleDisable(user, botManager, userManager);
    }
    else if (subCommand == "STATUS")
    {
        handleStatus(user, botManager, userManager);
    }
    else
    {
        if (!isOperator(user))
        {
            sendResponse(user, "Error: You need operator privileges to manage bots", userManager);
        }
        else
        {
            sendResponse(user, "Error: Unknown bot command. Use /BOT HELP for available commands", userManager);
        }
    }
    return true;
}

void BotCommand::handleList(User* user, BotManager* botManager, UserManager* userManager)
{
    std::vector<std::string> botList = botManager->getBotList();
    if (botList.empty())
    {
        sendResponse(user, "No bots are currently active", userManager);
        return;
    }

    sendResponse(user, "Active bots:", userManager);
    for (size_t i = 0; i < botList.size(); ++i)
    {
        IRCBot* bot = botManager->getBot(botList[i]);
        if (bot)
        {
            std::string status = bot->isActive() ? "active" : "inactive";
            std::ostringstream oss;
            oss << (i + 1) << ". " << bot->getNickname() << " (" << status << ")";
            sendResponse(user, oss.str(), userManager);
        }
    }
}

void BotCommand::handleCreate(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager)
{
    if (params.size() < 2)
    {
        sendResponse(user, "Usage: /BOT CREATE <nickname>", userManager);
        return;
    }
    
    std::string botNick = params[1];
    IRCBot* existingBot = botManager->getBot(botNick);
    if (existingBot)
    {
        sendResponse(user, "Error: Bot '" + botNick + "' already exists", userManager);
        return;
    }
    
    IRCBot* newBot = botManager->createBot(botNick);
    if (newBot)
    {
        sendResponse(user, "Bot '" + botNick + "' created successfully", userManager);
    }
    else
    {
        sendResponse(user, "Error: Failed to create bot '" + botNick + "'", userManager);
    }
}

void BotCommand::handleRemove(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager)
{
    if (params.size() < 2)
    {
        sendResponse(user, "Usage: /BOT REMOVE <nickname>", userManager);
        return;
    }
    std::string botNick = params[1];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
    {
        botManager->removeBot(botNick);
        sendResponse(user, "Bot '" + botNick + "' removed successfully", userManager);
    }
    else
    {
        sendResponse(user, "Error: Bot '" + botNick + "' not found", userManager);
    }
}

void BotCommand::handleJoin(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager)
{
    if (params.size() < 3)
    {
        sendResponse(user, "Usage: /BOT JOIN <nickname> <channel>", userManager);
        return;
    }
    std::string botNick = params[1];
    std::string channel = params[2];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
    {
        bot->joinChannel(channel);
        sendResponse(user, "Bot '" + botNick + "' joined channel '" + channel + "'", userManager);
    }
    else
    {
        sendResponse(user, "Error: Bot '" + botNick + "' not found", userManager);
    }
}

void BotCommand::handlePart(User* user, const std::vector<std::string>& params, BotManager* botManager, UserManager* userManager)
{
    if (params.size() < 3)
    {
        sendResponse(user, "Usage: /BOT PART <nickname> <channel>", userManager);
        return;
    }
    std::string botNick = params[1];
    std::string channel = params[2];
    IRCBot* bot = botManager->getBot(botNick);
    if (bot)
    {
        bot->leaveChannel(channel);
        sendResponse(user, "Bot '" + botNick + "' left channel '" + channel + "'", userManager);
    }
    else
    {
        sendResponse(user, "Error: Bot '" + botNick + "' not found", userManager);
    }
}

void BotCommand::handleEnable(User* user, BotManager* botManager, UserManager* userManager)
{
    botManager->enableBots();
    sendResponse(user, "Bot system enabled", userManager);
}

void BotCommand::handleDisable(User* user, BotManager* botManager, UserManager* userManager)
{
    botManager->disableBots();
    sendResponse(user, "Bot system disabled", userManager);
}

void BotCommand::handleStatus(User* user, BotManager* botManager, UserManager* userManager)
{
    std::string status = botManager->getStatus();
    sendResponse(user, status, userManager);
}

void BotCommand::handleHelp(User* user, UserManager* userManager)
{
    sendResponse(user, "Available BOT commands:", userManager);
    sendResponse(user, "/BOT LIST - List all active bots", userManager);
    sendResponse(user, "/BOT STATUS - Show bot system status", userManager);
    sendResponse(user, "/BOT HELP - Show this help message", userManager);
    if (isOperator(user))
    {
        sendResponse(user, "Operator commands:", userManager);
        sendResponse(user, "/BOT CREATE <nickname> - Create a new bot", userManager);
        sendResponse(user, "/BOT REMOVE <nickname> - Remove a bot", userManager);
        sendResponse(user, "/BOT JOIN <nickname> <channel> - Make bot join channel", userManager);
        sendResponse(user, "/BOT PART <nickname> <channel> - Make bot leave channel", userManager);
        sendResponse(user, "/BOT ENABLE - Enable the bot system", userManager);
        sendResponse(user, "/BOT DISABLE - Disable the bot system", userManager);
    }
    else
    {
        sendResponse(user, "Note: Operator privileges required for bot management", userManager);
    }
}

void BotCommand::sendResponse(User* user, const std::string& message, UserManager* userManager) 
{
    std::string response = ":localhost NOTICE " + user->getNickname() + " :" + message + "\r\n";
    userManager->sendMessage(user, response);
}

bool BotCommand::isOperator(User* user)
{
    // For now, all registered users are operators
    // In a real implementation, you'd check against an operator list
    return user->isRegistered();
}
