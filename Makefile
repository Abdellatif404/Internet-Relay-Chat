NAME = ircserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Wshadow -Wno-shadow -I./include
SRCS = src/server/main.cpp src/server/EventLoop.cpp \
		src/message/MessageBuffer.cpp src/message/SendQueue.cpp \
		src/server/Server.cpp src/network/SocketHandler.cpp src/network/EventHandler.cpp \
		src/network/ConnectionManager.cpp src/network/Connection.cpp \
		src/channel/Channel.cpp src/channel/ChannelManager.cpp \
		src/user/User.cpp src/user/UserManager.cpp \
		src/commands/user/UserCommand.cpp \
		src/commands/user/NickCommand.cpp \
		src/commands/user/PassCommand.cpp \
		src/commands/user/PingCommand.cpp \
		src/commands/user/PrivMsgCommand.cpp \
		src/commands/user/QuitCommand.cpp \
		src/commands/user/CapCommand.cpp \
		src/commands/channel/JoinCommand.cpp \
		src/commands/channel/PartCommand.cpp \
		src/commands/channel/TopicCommand.cpp \
		src/commands/channel/ModeCommand.cpp \
		src/commands/channel/KickCommand.cpp \
		src/commands/channel/InviteCommand.cpp \
		src/message/MessageParser.cpp \

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
