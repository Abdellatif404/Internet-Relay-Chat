
#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include "Server.hpp"

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#endif
