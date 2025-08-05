#include "MessageParser.hpp"

IRCMessage MessageParser::parse(const std::string& rawMessage)
{
    IRCMessage message;
    std::string line = trim(rawMessage);
    if (line.empty())
        return message;
    size_t pos = 0;
    if (line[0] == ':')
	{
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos)
		{
            message.prefix = line.substr(1, spacePos - 1);
            pos = spacePos + 1;
        }
		else
			return message;
    }
    while (pos < line.length() && std::isspace(line[pos]))
        pos++;
    size_t commandStart = pos;
    while (pos < line.length() && !std::isspace(line[pos]))
        pos++;
    if (pos > commandStart)
	{
        message.command = line.substr(commandStart, pos - commandStart);
    	std::transform(message.command.begin(), message.command.end(), message.command.begin(), ::toupper);
    }
    while (pos < line.length())
	{
        while (pos < line.length() && std::isspace(line[pos]))
            pos++;
        if (pos >= line.length())
            break;
		if (line[pos] == ':')
		{
            message.trailing = line.substr(pos + 1);
            message.params.push_back(message.trailing);
            break;
        }
        size_t paramStart = pos;
        while (pos < line.length() && !std::isspace(line[pos]))
            pos++;
        if (pos > paramStart)
            message.params.push_back(line.substr(paramStart, pos - paramStart));
    }
    return message;
}

std::string MessageParser::serialize(const IRCMessage& message)
{
    std::ostringstream oss;
    if (!message.prefix.empty())
        oss << ":" << message.prefix << " ";
    oss << message.command;
    for (size_t i = 0; i < message.params.size(); i++)
	{
        oss << " ";
        if (i == message.params.size() - 1 && (message.params[i].find(' ') != std::string::npos || message.params[i].empty() || message.params[i][0] == ':'))
            oss << ":" << message.params[i];
		else
            oss << message.params[i];
    }
    oss << "\r\n";
    return oss.str();
}

std::string MessageParser::trim(const std::string& str)
{
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(str[start]))
        start++;
    while (end > start && std::isspace(str[end - 1]))
        end--;
    return str.substr(start, end - start);
}

std::vector<std::string> MessageParser::split(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    
    while (std::getline(iss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}
