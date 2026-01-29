# IRC Server

## About

An IRC server written in C++98 implementing RFC 2812.

Features:
- Authentication
- Channels and channel modes
- Operators
- Bonus features:
  - IRC bot
  - DCC file transfer

## Requirements

- C++ compiler with C++98 support
- make
- UNIX-like operating system (Linux or macOS)

## Installation

Build the project using:

make

## Usage

Run the server with:

./ircserv <port> <password>

Example:

./ircserv 6667 mypassword

- port: Port number the server listens on
- password: Password required for client connections

## Connecting as a Client

You can connect using any IRC client.

Example with irssi:

irssi -c localhost -p 6667 -w mypassword

Example with netcat:

nc -C 127.0.0.1 6667
PASS mypassword
NICK mynick
USER myuser 0 * :Real Name

## Notes

- The server follows RFC 2812 specifications.
- Multiple clients can connect at the same time.
- Operator users have access to administrative commands.

## Author

IRC server project written in C++98.
