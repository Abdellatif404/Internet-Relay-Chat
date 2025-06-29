NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCDIR = src
OBJDIR = obj

SOURCES = $(SRCDIR)/main.cpp \
		$(SRCDIR)/user/User.cpp \
		$(SRCDIR)/user/UserManager.cpp \
		$(SRCDIR)/user/NickCommand.cpp \
		$(SRCDIR)/commands/user/PassCommand.cpp \
		$(SRCDIR)/commands/user/UserCommand.cpp

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re