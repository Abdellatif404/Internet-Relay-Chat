#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "User.hpp"
#include "SendQueue.hpp"

class UserManager {
private:
    std::map<int, User*> _users;           
    std::map<std::string, User*> _nicknames; 
    std::string _serverPassword;
    SendQueue* _sendQueue;
    std::string _serverName;
    std::string _serverVersion;
    int _maxUsers;
    std::vector<std::string> _operators;   
    
    
    bool _periodicStatsEnabled;
    time_t _lastStatsTime;

public:
    UserManager(const std::string& password, SendQueue* sendQueue, const std::string& serverName);
    UserManager(const std::string& password, SendQueue* sendQueue);  
    ~UserManager();

    
    User* createUser(int fd);
    void removeUser(int fd);
    User* getUser(int fd);
    User* getUserByNickname(const std::string& nickname);

    
    void handleDisconnection(int fd); 

    
    bool authenticateUser(User* user, const std::string& password);
    bool isNicknameAvailable(const std::string& nickname);
    bool registerNickname(User* user, const std::string& nickname);
    void unregisterNickname(const std::string& nickname);
    bool changeNickname(User* user, const std::string& newNickname);

    
    bool tryCompleteRegistration(User* user);
    void sendWelcomeMessages(User* user);
    
    
    bool validateUserState(User* user) const;
    void checkUserConsistency(User* user) const;

    
    void sendMessage(User* user, const std::string& message);
    void sendToUser(int fd, const std::string& message);
    void sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message);

    
    void broadcastToAll(const std::string& message, User* except = NULL);
    void setServerOperator(const std::string& nickname);
    bool isServerOperator(const std::string& nickname) const;

    
    std::vector<User*> getAllUsers() const;
    size_t getUserCount() const;
    
    
    void enablePeriodicStats(bool enable) { _periodicStatsEnabled = enable; }
    void checkAndPrintStats();

    
    std::string toLowerCase(const std::string& str) const;
    void handleUserQuit(User* user, const std::string& quitMessage = "");
    void pingUsers();
    void disconnectIdleUsers();

    
    void sendError(User* user, int errorCode, const std::string& errorText = "");
    void sendNumericReply(User* user, int code, const std::string& text);

    
    const std::string& getServerName() const { return _serverName; }
    const std::string& getServerVersion() const { return _serverVersion; }
    int getMaxUsers() const { return _maxUsers; }
    void setMaxUsers(int max) { _maxUsers = max; }
    
    
    void printMemoryStats() const;
};

#endif