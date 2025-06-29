
#include "Server.hpp"

Server::Server(int port, const std::string& password)
	: _port(port), _password(password)
{
}

Server::~Server()
{
}

void Server::start()
{
	std::cout << GREEN << "Starting IRC server on port " << _port << RESET << std::endl;
}
