
all:
	c++ -std=c++98 -Wall -Wextra -Werror -I./include src/*.cpp src/network/*.cpp src/user/*.cpp src/channel/*.cpp -o ircserv
clean:
	rm -f ircserv

fclean: clean

re: fclean all
