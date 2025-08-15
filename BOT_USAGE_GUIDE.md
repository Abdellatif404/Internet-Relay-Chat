# IRC Bot Commands Usage Guide

## Prerequisites
1. Connect to the IRC server: `nc -C localhost 6668`
2. Authenticate: `PASS password123`
3. Set nickname: `NICK your_nickname`
4. Register: `USER your_nickname 0 * :Your Real Name`
5. Join a channel: `JOIN #general`

## Bot Management Commands (BOT)

### 1. Check Bot System Status
```
BOT STATUS
```
**Example Output:**
```
:localhost NOTICE you :Bot system: enabled | Active bots: 2
```

### 2. List All Active Bots
```
BOT LIST
```
**Example Output:**
```
:localhost NOTICE you :Active bots:
:localhost NOTICE you :- HelpBot (active)
:localhost NOTICE you :- ChatBot (active)
```

### 3. Create a New Bot
```
BOT CREATE <bot_name>
```
**Examples:**
```
BOT CREATE HelpBot
BOT CREATE ChatAssistant
BOT CREATE GameBot
```
**Example Output:**
```
:localhost NOTICE you :Bot 'HelpBot' created successfully
```

### 4. Make Bot Join a Channel
```
BOT JOIN <bot_name> <channel>
```
**Examples:**
```
BOT JOIN HelpBot #general
BOT JOIN ChatAssistant #chat
BOT JOIN GameBot #games
```
**Example Output:**
```
:localhost NOTICE you :Bot 'HelpBot' joined channel #general
```

### 5. Make Bot Leave a Channel
```
BOT PART <bot_name> <channel>
```
**Examples:**
```
BOT PART HelpBot #general
BOT PART ChatAssistant #chat
```
**Example Output:**
```
:localhost NOTICE you :Bot 'HelpBot' left channel #general
```

### 6. Remove a Bot Completely
```
BOT REMOVE <bot_name>
```
**Examples:**
```
BOT REMOVE HelpBot
BOT REMOVE ChatAssistant
```
**Example Output:**
```
:localhost NOTICE you :Bot 'HelpBot' removed successfully
```

### 7. Enable/Disable Bot System
```
BOT ENABLE
BOT DISABLE
```
**Example Output:**
```
:localhost NOTICE you :Bot system enabled
```

### 8. Get Help for Bot Commands
```
BOT HELP
```
**Example Output:**
```
:localhost NOTICE you :Available BOT commands:
:localhost NOTICE you :/BOT LIST - List all active bots
:localhost NOTICE you :/BOT STATUS - Show bot system status
:localhost NOTICE you :/BOT CREATE <nickname> - Create a new bot
```

## Bot Chat Commands (!) - Interactive Commands

Once you have created a bot and it has joined a channel, you can interact with it using these commands:

### 1. Get Bot Help
```
PRIVMSG #general :!help
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Available commands:
:HelpBot!ircbot@bot.irc.server PRIVMSG you :!help - Display this help message
:HelpBot!ircbot@bot.irc.server PRIVMSG you :!time - Show current server time
:HelpBot!ircbot@bot.irc.server PRIVMSG you :!ping - Ping the bot
:HelpBot!ircbot@bot.irc.server PRIVMSG you :!joke - Tell a random joke
```

### 2. Get Current Time
```
PRIVMSG #general :!time
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Current server time: Fri Aug 15 21:30:45 2025
```

### 3. Ping the Bot
```
PRIVMSG #general :!ping
PRIVMSG #general :!ping Hello there!
PRIVMSG #general :!ping How are you doing?
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Pong!
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Pong: Hello there!
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Pong: How are you doing?
```

### 4. Echo Command
```
PRIVMSG #general :!echo Hello World!
PRIVMSG #general :!echo This is a test message
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Echo: Hello World!
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Echo: This is a test message
```

### 5. Get Bot Version
```
PRIVMSG #general :!version
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :IRC Bot v1.0 - Built for ft_irc project
```

### 6. Get Bot Uptime
```
PRIVMSG #general :!uptime
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Bot uptime: 0 days, 0 hours, 5 minutes, 23 seconds
```

### 7. Get Random Joke
```
PRIVMSG #general :!joke
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Why do programmers prefer dark mode? Because light attracts bugs!
```

### 8. Get Inspirational Quote
```
PRIVMSG #general :!quote
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :"First, solve the problem. Then, write the code." - John Johnson
```

### 9. Get User Information
```
PRIVMSG #general :!userinfo your_nickname
PRIVMSG #general :!userinfo alice
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :User alice (alice@localhost) - IRC Operator
```

### 10. Get Channel Information
```
PRIVMSG #general :!channelinfo
```
**Example Output:**
```
:HelpBot!ircbot@bot.irc.server PRIVMSG you :Channel: #general - Bot monitoring active
```

## Complete Example Session

Here's a complete example of how to use the bot system:

```bash
# Connect and authenticate
nc -C localhost 6668
PASS password123
NICK alice
USER alice 0 * :Alice User
JOIN #general

# Set up bots
BOT STATUS
BOT CREATE HelpBot
BOT CREATE ChatBot
BOT LIST
BOT JOIN HelpBot #general
BOT JOIN ChatBot #general
BOT ENABLE

# Interact with bots in channel
PRIVMSG #general :!help
PRIVMSG #general :!time
PRIVMSG #general :!ping Hello HelpBot!
PRIVMSG #general :!joke
PRIVMSG #general :!quote

# Direct message to bot
PRIVMSG HelpBot :!help
PRIVMSG ChatBot :!time

# Bot management
BOT PART HelpBot #general
BOT REMOVE ChatBot
BOT LIST

QUIT :Goodbye
```

## Important Notes

1. **Bot Commands (BOT)**: These manage the bot system itself (create, remove, join channels, etc.)
2. **Chat Commands (!)**: These are for interacting with active bots in channels
3. **Responses**: Bot responses to channel commands are sent as private messages to the user who issued the command
4. **Permissions**: All users can use chat commands (!), but only registered users can manage bots (BOT commands)
5. **Bot must be in channel**: For channel chat commands to work, a bot must first join that channel using `BOT JOIN`

## Troubleshooting

- If `!help` returns "Unknown command", make sure you have created a bot and made it join the channel
- If bot commands don't work, check that the bot system is enabled with `BOT STATUS`
- If no response from bots, verify they are active with `BOT LIST`
