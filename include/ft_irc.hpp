
#ifndef FT_IRC_HPP
#define FT_IRC_HPP

// Standard C++ Library
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iterator>

// Standard C Library
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cerrno>
#include <csignal>
#include <cctype>
#include <cstdio>

// POSIX Headers
#include <unistd.h>
#include <fcntl.h>

// Network Headers
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct sockaddr soaddr_t;

// IRC Response Codes (RFC 2812)
// Welcome messages
#define RPL_WELCOME         "001"
#define RPL_YOURHOST        "002"
#define RPL_CREATED         "003"
#define RPL_MYINFO          "004"
#define RPL_ISUPPORT        "005"

// User/nick info
#define RPL_UMODEIS         "221"
#define RPL_AWAY            "301"
#define RPL_UNAWAY          "305"
#define RPL_NOWAWAY         "306"

// Error codes
#define ERR_NOSUCHNICK      "401"
#define ERR_NOSUCHCHANNEL   "403"
#define ERR_CANNOTSENDTOCHAN "404"
#define ERR_TOOMANYCHANNELS "405"
#define ERR_UNKNOWNCOMMAND  "421"
#define ERR_NOMOTD          "422"
#define ERR_NONICKNAMEGIVEN "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE   "433"
#define ERR_NICKCOLLISION   "436"
#define ERR_NOTONCHANNEL    "442"
#define ERR_USERONCHANNEL   "443"
#define ERR_NOTREGISTERED   "451"
#define ERR_NEEDMOREPARAMS  "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH  "464"
#define ERR_CHANNELISFULL   "471"
#define ERR_UNKNOWNMODE     "472"
#define ERR_INVITEONLYCHAN  "473"
#define ERR_BANNEDFROMCHAN  "474"
#define ERR_BADCHANNELKEY   "475"
#define ERR_NOPRIVILEGES    "481"
#define ERR_CHANOPRIVSNEEDED "482"

// Color definitions for terminal output

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#endif
