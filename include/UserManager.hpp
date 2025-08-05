#ifndef USERMANAGER_HPP
#define USERMANAGER_HPP

#include "User.hpp"
#include "SendQueue.hpp"

class UserManager
{
	private:
		std::map<int, User*> _users;           // fd -> User
		std::map<std::string, User*> _nicknames; // nickname -> User (case-insensitive)
		std::string _serverPassword, _serverName, _serverVersion;
		SendQueue* _sendQueue;
		int _maxUsers;
		std::vector<std::string> _operators;
		
		static std::string toLowerCase(const std::string& str);
		void cleanupNickname(User* user);
		
	public:
		UserManager(const std::string& password, SendQueue* sendQueue, const std::string& serverName);
		UserManager(const std::string& password, SendQueue* sendQueue);
		~UserManager();
		
		// User management
		User* createUser(int fd);
		void removeUser(int fd);
		User* getUser(int fd) const;
		User* getUserByNickname(const std::string& nickname) const;
		void handleUserQuit(User* user, const std::string& quitMessage = "");
		void handleDisconnection(int fd) { removeUser(fd); }
		
		// Authentication and registration
		bool authenticateUser(User* user, const std::string& password);
		bool isNicknameAvailable(const std::string& nickname) const;
		bool updateNickname(User* user, const std::string& nickname);
		bool tryCompleteRegistration(User* user);
		
		// Messaging
		void sendMessage(User* user, const std::string& message);
		void sendToUser(int fd, const std::string& message);
		void sendPrivateMessage(User* sender, const std::string& targetNick, const std::string& message);
		void broadcastToAll(const std::string& message, User* except = NULL);
		void sendWelcomeMessages(User* user);
		void sendError(User* user, int errorCode, const std::string& errorText = "");
		void sendNumericReply(User* user, int code, const std::string& text);
		
		// Utilities
		std::vector<User*> getAllUsers() const;
		size_t getUserCount() const { return _users.size(); }
		void pingUsers();
		void disconnectIdleUsers();
		
		// Operators
		void setServerOperator(const std::string& nickname);
		bool isServerOperator(const std::string& nickname) const;
		
		// Getters
		const std::string& getServerName() const { return _serverName; }
		const std::string& getServerVersion() const { return _serverVersion; }
		int getMaxUsers() const { return _maxUsers; }
		void setMaxUsers(int max) { _maxUsers = max; }
};

#endif