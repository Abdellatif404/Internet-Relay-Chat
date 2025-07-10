
#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <climits>
#include <vector>
#include <fcntl.h>
#include <set>
#include <map>
#include <algorithm>
#include <csignal>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct sockaddr soaddr_t;

#include "User.hpp"
#include "UserManager.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "Connection.hpp"
#include "EventLoop.hpp"
#include "NickCommand.hpp"
#include "PassCommand.hpp"
#include "Server.hpp"
#include "UserCommand.hpp"
#include "SocketHandler.hpp"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#endif
