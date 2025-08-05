#ifndef USER_HPP
#define USER_HPP

#include "ft_irc.hpp"

enum UserState {
    UNREGISTERED,
    PASS_SET,
    NICK_SET,
    REGISTERED
};

class User
{
	private:
		int _fd;
		std::string _nickname, _username, _realname, _hostname, _awayMessage;
		UserState _state;
		time_t _lastPing, _connectionTime;
		bool _authenticated, _isOperator, _isAway;

	public:
		User(int fd);
		~User() {}
		
		// Getters
		int getFd() const { return _fd; }
		const std::string& getNickname() const { return _nickname; }
		const std::string& getUsername() const { return _username; }
		const std::string& getRealname() const { return _realname; }
		const std::string& getHostname() const { return _hostname; }
		UserState getState() const { return _state; }
		bool isAuthenticated() const { return _authenticated; }
		bool isRegistered() const { return _state == REGISTERED; }
		time_t getLastPing() const { return _lastPing; }
		time_t getConnectionTime() const { return _connectionTime; }
		bool isOperator() const { return _isOperator; }
		const std::string& getAwayMessage() const { return _awayMessage; }
		bool isAway() const { return _isAway; }
		
		// Setters
		void setNickname(const std::string& nickname) { _nickname = nickname; }
		void setUsername(const std::string& username) { _username = username; }
		void setRealname(const std::string& realname) { _realname = realname; }
		void setHostname(const std::string& hostname) { _hostname = hostname; }
		void setState(UserState state) { _state = state; }
		void setAuthenticated(bool auth) { _authenticated = auth; }
		void setOperator(bool op) { _isOperator = op; }
		void setAway(bool away, const std::string& message = "");
		
		// Utility functions
		std::string getPrefix() const;
		static bool isValidNickname(const std::string& nick);
		static bool isValidUsername(const std::string& username);
		void updateLastPing() { _lastPing = time(NULL); }
		bool needsPing() const { return (time(NULL) - _lastPing) > 60; }
};

#endif