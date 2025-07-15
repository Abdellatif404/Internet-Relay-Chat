NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Wshadow -Wno-shadow -I./include
SRCS = src/main.cpp src/EventLoop.cpp \
		src/network/MessageBuffer.cpp src/network/SendQueue.cpp \
		src/network/Server.cpp src/network/SocketHandler.cpp \
		src/network/ConnectionManager.cpp src/network/Connection.cpp \
		src/channel/Channel.cpp src/channel/ChannelManager.cpp \
		src/user/User.cpp src/user/UserManager.cpp \
		src/commands/user/UserCommand.cpp \
		src/commands/user/NickCommand.cpp \
		src/commands/user/PassCommand.cpp \

OBJ_DIR = obj
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: compile_message $(NAME)

compile_message:
	@echo "⚙️  Compiling ...."

$(NAME): $(OBJS)
	@echo "🔗 Linking ...."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Build complete!"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "🧹 Cleaning object files ...."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "🗑️  Removing executable ...."
	@rm -rf $(NAME)

re: fclean all
