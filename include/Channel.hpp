#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "User.hpp"

class Channel {
private:
	std::string name;
	std::string topic;
	std::string key;
	std::set<User*> members;
	std::set<User*> operators;
	bool inviteOnly;
	bool topicRestricted;
	int userLimit;

public:
	Channel(const std::string& name);
	~Channel();

	void addMember(User* user);
	void removeMember(User* user);
	bool isMember(User* user) const;
	void broadcast(const std::string& msg, User* sender);

	void addOperator(User* user);
	bool isOperator(User* user) const;

	const std::string& getName() const;
};

#endif