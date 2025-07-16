#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "User.hpp"
#include "SendQueue.hpp"

class UserManager {
private:
    std::map<int, User*> _users;           // fd -> User
    std::map<std::string, User*> _nicknames; // nickname -> User (case-insensitive)
    std::string _serverPassword;
    SendQueue* _sendQueue;
    std::string _serverName;
    std::string _serverVersion;
    int _maxUsers;
    std::vector<std::string> _operators;   // Server operators list

public:
    UserManager(const std::string& password, SendQueue* sendQueue, const std::string& serverName);
    UserManager(const std::string& password, SendQueue* sendQueue);  // Overload for EventLoop compatibility
    ~UserManager();

    // User lifecycle
    User* createUser(int fd);
    void removeUser(int fd);
    User* getUser(int fd);
    User* getUserByNickname(const std::string& nickname);

    // Authentication
    bool authenticateUser(User* user, const std::string& password);
    bool isNicknameAvailable(const std::string& nickname);
    bool registerNickname(User* user, const std::string& nickname);
    void unregisterNickname(const std::string& nickname);
    bool changeNickname(User* user, const std::string& newNickname);

    // User registration flow
    bool tryCompleteRegistration(User* user);
    void sendWelcomeMessages(User* user);

    // Message sending
    void sendMessage(User* user, const std::string& message);
    void sendToUser(int fd, const std::string& message);
    void sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message);

    // Server operations
    void broadcastToAll(const std::string& message, User* except = NULL);
    void setServerOperator(const std::string& nickname);
    bool isServerOperator(const std::string& nickname) const;

    // User queries
    std::vector<User*> getAllUsers() const;
    size_t getUserCount() const;

    // Utility
    std::string toLowerCase(const std::string& str) const;
    void handleUserQuit(User* user, const std::string& quitMessage = "");
    void pingUsers();
    void disconnectIdleUsers();

    // Error handling
    void sendError(User* user, int errorCode, const std::string& errorText = "");
    void sendNumericReply(User* user, int code, const std::string& text);

    // Server info
    const std::string& getServerName() const { return _serverName; }
    const std::string& getServerVersion() const { return _serverVersion; }
    int getMaxUsers() const { return _maxUsers; }
    void setMaxUsers(int max) { _maxUsers = max; }
};

#endif