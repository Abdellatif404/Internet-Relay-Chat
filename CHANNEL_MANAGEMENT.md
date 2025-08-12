# Channel Management System - ft_irc

This document describes the enhanced channel management system for the ft_irc project.

## Overview

The channel management system has been completely redesigned to provide a robust, IRC-compliant implementation with proper command handling, mode management, and user permissions.

## Key Components

### 1. Enhanced Channel Class (`Channel.hpp` / `Channel.cpp`)

**New Features:**
- Complete IRC channel mode support (i, t, m, n, s, p, k, l, o)
- Topic management with who/when tracking
- Invite list functionality
- Enhanced permission checks
- Proper message broadcasting
- Member list generation
- Mode string formatting

**Supported Modes:**
- `+i` - Invite-only channel
- `+t` - Topic restricted to operators
- `+m` - Moderated channel
- `+n` - No external messages
- `+s` - Secret channel
- `+p` - Private channel
- `+k` - Channel key (password)
- `+l` - User limit
- `+o` - Channel operator

### 2. Enhanced ChannelManager Class (`ChannelManager.hpp` / `ChannelManager.cpp`)

**New Features:**
- Channel name validation
- User removal from all channels
- Channel cleanup functionality
- Enhanced user-to-channel mapping
- Key-based channel joining

### 3. IRC Channel Commands

#### JoinCommand (`JoinCommand.hpp` / `JoinCommand.cpp`)
- Handles JOIN command with multiple channels
- Channel key support
- Proper permission checks
- NAMES list generation
- Topic display on join

#### PartCommand (`PartCommand.hpp` / `PartCommand.cpp`)
- Handles PART command with multiple channels
- Optional part message support
- Proper message broadcasting

#### TopicCommand (`TopicCommand.hpp` / `TopicCommand.cpp`)
- Topic viewing and setting
- Operator permission checks
- Topic history tracking (who/when)
- Proper error handling

#### ModeCommand (`ModeCommand.hpp` / `ModeCommand.cpp`)
- Complete channel mode management
- Multiple mode changes in single command
- Proper argument parsing
- User and channel mode support

#### KickCommand (`KickCommand.hpp` / `KickCommand.cpp`)
- User kicking functionality
- Operator permission checks
- Optional kick reason
- Proper message broadcasting

#### InviteCommand (`InviteCommand.hpp` / `InviteCommand.cpp`)
- User invitation to channels
- Invite-only channel support
- Proper permission checks
- Invite list management

## IRC Response Codes Added

```cpp
#define RPL_NOTOPIC         "331"  // No topic is set
#define RPL_TOPIC           "332"  // Channel topic
#define RPL_TOPICWHOTIME    "333"  // Topic who/when info
#define RPL_INVITING        "341"  // Invite successful
#define RPL_NAMREPLY        "353"  // Names list
#define RPL_ENDOFNAMES      "366"  // End of names list
#define RPL_CHANNELMODEIS   "324"  // Channel mode string
```

## Usage Examples

### Basic Channel Operations

```irc
JOIN #general                    // Join channel
JOIN #private secretkey          // Join with key
PART #general                    // Leave channel
PART #general :Goodbye!          // Leave with message
TOPIC #general                   // View topic
TOPIC #general :New topic here   // Set topic
```

### Channel Mode Management

```irc
MODE #general                    // View current modes
MODE #general +i                 // Set invite-only
MODE #general +k secretkey       // Set channel key
MODE #general +l 50              // Set user limit
MODE #general +o username        // Give operator status
MODE #general -i                 // Remove invite-only
```

### Administrative Commands

```irc
KICK #general baduser :Reason    // Kick user
INVITE gooduser #private         // Invite to channel
```

## Implementation Notes

### Thread Safety
- All operations are designed for single-threaded event loop
- No mutexes required with current architecture

### Memory Management
- Proper RAII with constructors/destructors
- Automatic cleanup of empty channels
- Smart pointer-like behavior for user references

### Error Handling
- Comprehensive IRC error code responses
- Proper parameter validation
- Permission checking at every level

### Standards Compliance
- RFC 2812 compliant responses
- Standard IRC channel naming conventions
- Proper message formatting

## Integration Points

### With UserManager
- User lookup by nickname (for KICK/INVITE commands)
- User authentication status checks
- User permission management

### With MessageParser
- Command parsing and parameter extraction
- Message routing to appropriate handlers

### With SendQueue
- Asynchronous message delivery
- Proper message queueing for disconnected users

## Building and Testing

```bash
make                           // Build the project
./ircserv <port> <password>   // Run the server
```

### Test Commands
```bash
# Connect with IRC client (e.g., irssi, hexchat)
/connect localhost <port> <password>
/nick testuser
/user testuser 0 * :Test User
/join #test
/mode #test
/topic #test :Test topic
/part #test :Leaving
```

## Future Enhancements

1. **Ban List Management** (`+b` mode)
2. **Exception List** (`+e` mode)  
3. **Voice Status** (`+v` mode)
4. **Channel Statistics**
5. **Channel Logging**
6. **Custom Channel Types**

## Dependencies

- C++98 standard compliance
- POSIX socket API
- Standard Template Library (STL)

## Files Modified/Added

### Header Files
- `include/Channel.hpp` (enhanced)
- `include/ChannelManager.hpp` (enhanced)
- `include/JoinCommand.hpp` (new)
- `include/PartCommand.hpp` (new)
- `include/TopicCommand.hpp` (new)
- `include/ModeCommand.hpp` (new)
- `include/KickCommand.hpp` (new)
- `include/InviteCommand.hpp` (new)
- `include/ft_irc.hpp` (enhanced with new response codes)

### Implementation Files
- `src/channel/Channel.cpp` (enhanced)
- `src/channel/ChannelManager.cpp` (enhanced)
- `src/commands/channel/JoinCommand.cpp` (new)
- `src/commands/channel/PartCommand.cpp` (new)
- `src/commands/channel/TopicCommand.cpp` (new)
- `src/commands/channel/ModeCommand.cpp` (new)
- `src/commands/channel/KickCommand.cpp` (new)
- `src/commands/channel/InviteCommand.cpp` (new)

### Build System
- `Makefile` (updated with new source files)

This channel management system provides a solid foundation for a fully-featured IRC server that can handle complex channel operations while maintaining compatibility with standard IRC clients.
