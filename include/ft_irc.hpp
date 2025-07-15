
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

// Color definitions for terminal output
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

// Project Headers
// Core Classes
#include "Server.hpp"
#include "EventLoop.hpp"

// Network Classes
#include "SocketHandler.hpp"
#include "Connection.hpp"
#include "ConnectionManager.hpp"
#include "MessageBuffer.hpp"
#include "SendQueue.hpp"

// Message Processing
#include "MessageParser.hpp"

// User Management
#include "User.hpp"
#include "UserManager.hpp"

// Channel Management
#include "Channel.hpp"
#include "ChannelManager.hpp"

// Command Classes
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "PingCommand.hpp"
#include "QuitCommand.hpp"
#include "PrivMsgCommand.hpp"

#endif
