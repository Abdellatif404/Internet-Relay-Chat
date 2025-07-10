
#include "ft_irc.hpp"

Server *g_server = NULL;

void	signalHandler(int signal)
{
	(void)signal;
	if (g_server)
	{
		std::cout << "\n" << RED << "Server shutting down..." << RESET << std::endl;
		delete g_server;
		g_server = NULL;
	}
	exit(0);
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

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << RED << "Usage: " << av[0] << " <port> <password>" << RESET << std::endl;
		return 1;
	}
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	try
	{
		uint16_t	port = validate_port(av[1]);
		std::string password = av[2];

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
