#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <cstring>
#include <sstream>

#include "user/UserManager.hpp"
#include "user/User.hpp"
#include "commands/user/PassCommand.hpp"
#include "commands/user/UserCommand.hpp"
#include "user/NickCommand.hpp"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 512

class SimpleServer {
private:
    int _serverFd;
    int _port;
    UserManager* _userManager;
    std::vector<struct pollfd> _pollfds;

public:
    SimpleServer(int port, const std::string& password) : _serverFd(-1), _port(port) {
        _userManager = new UserManager(password);
    }

    ~SimpleServer() {
        delete _userManager;
        if (_serverFd != -1) {
            close(_serverFd);
        }
    }

    bool start() {
        // Create socket
        _serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (_serverFd == -1) {
            std::cerr << "Error: Failed to create socket" << std::endl;
            return false;
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Error: Failed to set socket options" << std::endl;
            return false;
        }

        // Set non-blocking
        if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0) {
            std::cerr << "Error: Failed to set non-blocking" << std::endl;
            return false;
        }

        // Bind socket
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(_port);

        if (bind(_serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "Error: Failed to bind socket" << std::endl;
            return false;
        }

        // Listen
        if (listen(_serverFd, 10) < 0) {
            std::cerr << "Error: Failed to listen" << std::endl;
            return false;
        }

        std::cout << "Server listening on port " << _port << std::endl;

        // Add server socket to poll
        struct pollfd serverPoll;
        serverPoll.fd = _serverFd;
        serverPoll.events = POLLIN;
        _pollfds.push_back(serverPoll);

        return true;
    }

    void run() {
        while (true) {
            std::cout << "Polling " << _pollfds.size() << " file descriptors..." << std::endl; // ADD THIS
            
            int pollCount = poll(&_pollfds[0], _pollfds.size(), -1);
            if (pollCount < 0) {
                std::cerr << "Error: poll() failed" << std::endl;
                break;
            }

            std::cout << "Poll returned: " << pollCount << " events" << std::endl; // ADD THIS

            for (size_t i = 0; i < _pollfds.size(); i++) {
                if (_pollfds[i].revents & POLLIN) {
                    std::cout << "Event on fd " << _pollfds[i].fd << std::endl; // ADD THIS
                    
                    if (_pollfds[i].fd == _serverFd) {
                        // New connection
                        acceptNewConnection();
                    } else {
                        // Handle client data
                        handleClientData(_pollfds[i].fd);
                    }
                }
            }
        }
    }

private:
    void acceptNewConnection() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientFd = accept(_serverFd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            std::cerr << "Error: Failed to accept connection" << std::endl;
            return;
        }

        // Set non-blocking
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
            std::cerr << "Error: Failed to set client non-blocking" << std::endl;
            close(clientFd);
            return;
        }

        // Create user and use it
        _userManager->createUser(clientFd);
        std::cout << "New client connected: " << clientFd << std::endl;

        // Add to poll
        struct pollfd clientPoll;
        clientPoll.fd = clientFd;
        clientPoll.events = POLLIN;
        _pollfds.push_back(clientPoll);
    }

    void handleClientData(int clientFd) {
        char buffer[BUFFER_SIZE];
        int bytesRead = recv(clientFd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytesRead <= 0) {
            std::cout << "Client disconnected: " << clientFd << std::endl;
            disconnectClient(clientFd);
            return;
        }

        buffer[bytesRead] = '\0';
        
        User* user = _userManager->getUser(clientFd);
        if (!user) {
            std::cerr << "Error: User not found for fd " << clientFd << std::endl;
            return;
        }

        user->appendToBuffer(std::string(buffer));

        while (user->hasCompleteMessage()) {
            std::string message = user->extractMessage();
            processMessage(user, message);
        }
    }

    void processMessage(User* user, const std::string& message) {
        std::cout << "Received from " << user->getFd() << ": " << message << std::endl;

        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(message);
        
        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) return;

        std::string command = tokens[0];
        std::vector<std::string> params(tokens.begin() + 1, tokens.end());

        if (command == "PASS") {
            PassCommand::execute(user, params, _userManager);
        }
        else if (command == "NICK") {
            NickCommand::execute(user, params, _userManager);
        }
        else if (command == "USER") {
            UserCommand::execute(user, params, _userManager);
        }
        else if (command == "QUIT") {
            std::string quitMsg = ":localhost QUIT :Client quit\r\n";
            send(user->getFd(), quitMsg.c_str(), quitMsg.length(), 0);
            disconnectClient(user->getFd());
        }
        else {
            std::string error = ":localhost 421 * " + command + " :Unknown command\r\n";
            send(user->getFd(), error.c_str(), error.length(), 0);
        }
    }

    void disconnectClient(int clientFd) {
        // Remove from poll
        for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
            if (it->fd == clientFd) {
                _pollfds.erase(it);
                break;
            }
        }

        // Remove user
        _userManager->removeUser(clientFd);
        close(clientFd);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        std::cerr << "Error: Invalid port number" << std::endl;
        return 1;
    }

    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Error: Password cannot be empty" << std::endl;
        return 1;
    }

    SimpleServer server(port, password);
    
    if (!server.start()) {
        std::cerr << "Error: Failed to start server" << std::endl;
        return 1;
    }

    std::cout << "Starting IRC server..." << std::endl;
    server.run();

    return 0;
}