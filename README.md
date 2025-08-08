# IRC Server with Bot System

A complete IRC server implementation written in C++98 with an integrated bot management system.

## 🚀 Quick Start

### Building the Project

```bash
# Clone or navigate to the project directory
cd Internet-Relay-Chat

# Build the server
make

# Clean build (if needed)
make re
```

### Running the Server

```bash
# Start the IRC server
./ircserv <port> <password>

# Example:
./ircserv 6667 mypassword
```

**Parameters:**
- `<port>`: Port number for the server to listen on (e.g., 6667)
- `<password>`: Server password for user authentication

## 🔌 Connecting to the Server

### Using IRC Clients

#### HexChat (Recommended)
1. Install HexChat: `sudo apt install hexchat`
2. Open HexChat
3. In Network List, click **Add**
4. Name: `MyIRCServer`
5. Add server: `localhost/6667` (replace 6667 with your port)
6. Set server password if required
7. Click **Connect**

#### irssi (Terminal)
```bash
irssi
/connect localhost 6667 mypassword
```

#### telnet (Basic Testing)
```bash
telnet localhost 6667
```

### Basic IRC Commands

Once connected, authenticate and register:

```irc
PASS mypassword
NICK yournickname
USER username 0 * :Your Real Name
```

## 🤖 Bot System Usage

### Bot Management Commands (Operator Only)

```irc
/BOT STATUS                    # Show bot system status
/BOT LIST                      # List all active bots
/BOT CREATE <nickname>         # Create a new bot
/BOT REMOVE <nickname>         # Remove a bot
/BOT JOIN <nickname> <channel> # Make bot join a channel
/BOT PART <nickname> <channel> # Make bot leave a channel
/BOT ENABLE                    # Enable the bot system
/BOT DISABLE                   # Disable the bot system
/BOT HELP                      # Show available commands
```

**Examples:**
```irc
/BOT CREATE HelpBot
/BOT JOIN HelpBot #general
/BOT LIST
```

### Bot Interaction Commands

Interact with bots using `!` commands:

```irc
!help                          # Show available bot commands
!time                          # Show current server time
!ping [message]                # Ping the bot with optional message
!echo <message>                # Echo back your message
!version                       # Show bot version information
!uptime                        # Show bot uptime
!userinfo <nickname>           # Get information about a user
!channelinfo                   # Get current channel information
!joke                          # Tell a random programming joke
!quote                         # Share an inspirational programming quote
```

**Examples:**
```irc
!help
!ping Hello World
!echo This is a test message
!joke
```

## 📋 Step-by-Step Tutorial

### 1. Start the Server
```bash
make
./ircserv 6667 mypass
```

### 2. Connect with HexChat
- Open HexChat
- Create new network: `localhost/6667`
- Set password: `mypass`
- Connect

### 3. Test Basic IRC Features
```irc
/join #general
/msg someone Hello there
/nick newnickname
```

### 4. Test Bot System
```irc
/BOT STATUS
/BOT CREATE TestBot
/BOT JOIN TestBot #general
!help
!time
!ping Testing the bot
```

### 5. Advanced Bot Management
```irc
/BOT CREATE AdminBot
/BOT JOIN AdminBot #admin
/BOT LIST
!userinfo yournick
!joke
```

## 🛠️ Development and Debugging

### Build Targets

```bash
make          # Build the project
make clean    # Remove object files
make fclean   # Remove executable and objects
make re       # Clean rebuild
```

### Testing Multiple Connections

Open multiple terminals and connect different clients:

**Terminal 1:**
```bash
./ircserv 6667 mypass
```

**Terminal 2:**
```bash
irssi
/connect localhost 6667 mypass
/nick Alice
```

**Terminal 3:**
```bash
telnet localhost 6667
PASS mypass
NICK Bob
USER bob 0 * :Bob Smith
```

### Debugging Tips

1. **Check server logs:** The server prints detailed information to stdout
2. **Monitor connections:** Watch for connection/disconnection messages
3. **Bot responses:** Bot interactions are logged to the console
4. **Network errors:** Check if the port is already in use

```bash
# Check if port is in use
sudo netstat -tulnp | grep 6667

# Kill process using the port (if needed)
sudo kill -9 <PID>
```

## 🔧 Troubleshooting

### Common Issues

#### "BOT :Unknown command"
- **Solution:** Make sure the server was built with the latest bot system integration
- **Fix:** Run `make re` to rebuild

#### "Connection refused"
- **Solution:** Ensure the server is running and using the correct port
- **Check:** Verify no firewall is blocking the port

#### "Password required"
- **Solution:** Use the correct server password in your IRC client
- **Format:** `/server localhost 6667 password`

#### Bot not responding
- **Check:** `/BOT STATUS` to verify bot system is enabled
- **Verify:** `/BOT LIST` to see if bots are active
- **Test:** Use `!help` command to check bot responsiveness

### Server Management

```bash
# Start server in background
./ircserv 6667 mypass &

# Stop server
killall ircserv

# Monitor server activity
./ircserv 6667 mypass | tee server.log
```

## 📁 Project Structure

```
Internet-Relay-Chat/
├── src/
│   ├── main.cpp              # Server entry point
│   ├── EventLoop.cpp         # Main event handling
│   ├── bot/
│   │   ├── IRCBot.cpp        # Bot implementation
│   │   └── BotManager.cpp    # Bot management system
│   ├── commands/user/
│   │   ├── BotCommand.cpp    # Bot administration commands
│   │   ├── NickCommand.cpp   # Nickname management
│   │   ├── UserCommand.cpp   # User registration
│   │   └── ...
│   ├── network/              # Network layer
│   ├── channel/              # Channel management
│   └── user/                 # User management
├── include/                  # Header files
├── obj/                      # Compiled objects (generated)
├── Makefile                  # Build configuration
└── ircserv                   # Compiled executable (generated)
```

## 🎯 Features

- **Full IRC Protocol Support:** NICK, USER, JOIN, PART, PRIVMSG, QUIT, PING
- **Multi-Client Support:** Handle multiple simultaneous connections
- **Channel Management:** Create and manage IRC channels
- **Bot System:** Automated bots with custom commands and responses
- **User Authentication:** Password-based server access
- **Real-time Messaging:** Instant message delivery
- **Operator Commands:** Administrative bot management
- **C++98 Compliance:** Compatible with older C++ standards

## 📝 Example Session

```irc
# Server startup
$ ./ircserv 6667 secret123

# Client connection and basic usage
PASS secret123
NICK Alice
USER alice 0 * :Alice Johnson
:localhost 001 Alice :Welcome to the IRC Network Alice!johnson@localhost

# Join channel and interact
JOIN #general
:Alice!alice@localhost JOIN #general

# Create and test bots
/BOT CREATE WelcomeBot
Bot 'WelcomeBot' created successfully

/BOT JOIN WelcomeBot #general
Bot 'WelcomeBot' joined channel #general

!help
Available commands:
!help - Display this help message
!time - Show current server time
!ping - Ping the bot (usage: !ping [message])
...

!time
Current server time: Mon Aug  6 15:30:45 2025
```

## 🤝 Contributing

1. Follow C++98 standards
2. Add comprehensive error handling
3. Document new features
4. Test with multiple IRC clients
5. Ensure bot commands are intuitive

## 📜 License

This project is part of the ft_irc curriculum and follows school guidelines.

---

**Happy IRC chatting! 🎉**
