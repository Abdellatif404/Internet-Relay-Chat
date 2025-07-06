#include "Channel.hpp"
#include "User.hpp"

// --- Constructor ---
Channel::Channel(const std::string& name) : _name(name) {
    _topic = "";
    _key = "";
    _inviteOnly = false;
    _topicRestricted = true;
    _userLimit = 0; // 0 means no limit
}

// --- Destructor ---
Channel::~Channel() {
}

// --- User Management ---
void Channel::addUser(User* user) {
    if (_members.empty()) {
        _operators.insert(user);
    }
    _members.insert(user);
}

void Channel::removeUser(User* user) {
    _members.erase(user);
    _operators.erase(user);
}

void Channel::broadcastMessage(const std::string& message, User* sender) {
    for (std::set<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != sender) {
            // In the real server, you'd call a function to send the message
            // e.g., server->sendToClient((*it)->getFd(), message);
        }
    }
}

// --- Getters ---
const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

size_t Channel::getMemberCount() const {
    return _members.size();
}

// --- Operator & Member Checks ---
bool Channel::isOperator(User* user) const {
    return _operators.count(user) == 1;
}

bool Channel::isMember(User* user) const {
    return _members.count(user) == 1;
}

// --- Mode Management Functions ---
void Channel::setInviteOnly(bool status) {
    _inviteOnly = status;
}

void Channel::setTopicRestricted(bool status) {
    _topicRestricted = status;
}

void Channel::setKey(const std::string& key) {
    _key = key;
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

void Channel::addOperator(User* user) {
    if (isMember(user)) {
        _operators.insert(user);
    }
}

void Channel::removeOperator(User* user) {
    _operators.erase(user);
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

// --- Getters for Modes & Attributes ---
bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

const std::string& Channel::getKey() const {
    return _key;
}

int Channel::getUserLimit() const {
    return _userLimit;
}