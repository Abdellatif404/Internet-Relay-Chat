#ifndef MESSAGEPARSER_HPP
#define MESSAGEPARSER_HPP

#include "ft_irc.hpp"

struct IRCMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
    
    IRCMessage() {}
    IRCMessage(const std::string& cmd) : command(cmd) {}
};

class MessageParser {
public:
    static IRCMessage parse(const std::string& rawMessage);
    static std::string serialize(const IRCMessage& message);
    
private:
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif
