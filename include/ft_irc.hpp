
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

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "User.hpp"
#include "UserManager.hpp"
#include "Channel.hpp"
#include "ChannelManager.hpp"
#include "EventLoop.hpp"
#include "NickCommand.hpp"
#include "PassCommand.hpp"
#include "Server.hpp"
#include "UserCommand.hpp"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#endif
