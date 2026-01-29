
#include "ft_irc.hpp"
#include "Server.hpp"

Server *g_server = NULL;
bool	g_running = false;

void	signalHandler(int signal)
{
	(void)signal;
	g_running = false;
	if (g_server)
		std::cerr << RED << "Server shutting down..." << RESET << std::endl;
}

int	validate_port(char *str)
{
	int i = 0;
	int num_len = 0;
	int zeros_len = 0;

	while (str[i] && std::isspace(str[i]))
		i++;
	if (str[i] == '+')
		i++;
	while (str[i] && str[i] == '0')
	{
		zeros_len++;
		i++;
	}
	while (str[i] && std::isdigit(str[i]))
	{
		num_len++;
		i++;
	}
	while (str[i] && std::isspace(str[i]))
		i++;
	if (str[i] != '\0' || num_len + zeros_len == 0)
		throw std::invalid_argument("Port invalid!");

	int	port = std::atoi(str);
	if (port <= 0 || port > 65535 || num_len > 5)
		throw std::out_of_range("Port must be between 1 and 65535");
	return port;
}

std::string validate_password(const char *arg)
{
	std::string password(arg);
	if (password.empty() || password.find(' ') != std::string::npos)
		throw std::invalid_argument("Password cannot be empty or contain spaces!");
	if (password.length() > 64)
		throw std::length_error("Password too long! Maximum length is 64 characters.");
	return password;
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << RED << "Usage: " << av[0] << " <port> <password>" << RESET << std::endl;
		return 1;
	}
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	try
	{
		uint16_t	port = validate_port(av[1]);
		std::string	password = validate_password(av[2]);

		g_server = new Server(port, password);
		g_server->start();
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		delete g_server;
		return 1;
	}
	delete g_server;
	return (0);
}
