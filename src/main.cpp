
#include "ft_irc.hpp"

int	validate_port(char *str)
{
	for (int i = 0; str[i]; i++)
	{
		if (std::isspace(str[i]))
			i++;
		if (str[i] == '+')
			i++;
		if (!std::isdigit(str[i]))
		{
			std::cerr << RED << "Error: Port invalid!" << RESET << std::endl;
			exit(1);
		}
	}
	int	port = std::atoi(str);
	if (port <= 0 || port > 65535)
	{
		std::cerr << RED << "Error: Port must be between 1 and 65535" << RESET << std::endl;
		exit(1);
	}
	return port;
}

int	main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << RED << "Usage: " << av[0] << " <port> <password>" << RESET << std::endl;
		return 1;
	}
	int			port = validate_port(av[1]);
	std::string password = av[2];

	try
	{
		Server server(port, password);
		server.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
		return 1;
	}
	return (0);
}
