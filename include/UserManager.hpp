#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "User.hpp"
#include "SendQueue.hpp"

class UserManager {
private:
    std::map<int, User*> _users;           // fd -> User
    std::map<std::string, User*> _nicknames; // nickname -> User
    std::string _serverPassword;
	SendQueue* _sendQueue;

public:
    UserManager(const std::string& password, SendQueue* sendQueue);
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

    // User registration flow
    bool tryCompleteRegistration(User* user);
    void sendWelcomeMessages(User* user);

    // Message sending
    void sendMessage(User* user, const std::string& message);

    // Utility
    std::vector<User*> getAllUsers() const;
    size_t getUserCount() const;
};

#endif