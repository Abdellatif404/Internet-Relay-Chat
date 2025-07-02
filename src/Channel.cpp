#include "Channel.hpp"

Channel::Channel(const std::string& name) : name(name), inviteOnly(false), topicRestricted(false), userLimit(-1) {}

Channel::~Channel() {}

void Channel::addMember(User* user) {
	members.insert(user);
}

void Channel::removeMember(User* user) {
	members.erase(user);
}

bool Channel::isMember(User* user) const {
	return members.find(user) != members.end();
}

void Channel::broadcast(const std::string& msg, User* sender) {
	for (std::set<User*>::iterator it = members.begin(); it != members.end(); ++it) {
		if (*it != sender)
			(*it)->send(msg);
	}
}

void Channel::addOperator(User* user) {
	operators.insert(user);
}

bool Channel::isOperator(User* user) const {
	return operators.find(user) != operators.end();
}

const std::string& Channel::getName() const {
	return name;
}
