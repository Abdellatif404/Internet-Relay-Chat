#include "Channel.hpp"

// --- Constructor ---
Channel::Channel(const std::string& name, SendQueue* sendQueue)
	: _name(name), _sendQueue(sendQueue) {
    _topic = "";
    _key = "";
    _topicWho = "";
    _topicTime = 0;
    _inviteOnly = false;
    _topicRestricted = true;
    _moderated = false;
    _noExternalMessages = false;
    _secret = false;
    _private = false;
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
    _invited.erase(user); // Remove from invited list if they were invited
}

void Channel::removeUser(User* user) {
    _members.erase(user);
    _operators.erase(user);
    _invited.erase(user);
}

void Channel::kickUser(User* user, User* kicker, const std::string& reason) {
    if (isMember(user)) {
        removeUser(user);
        // Broadcast kick message to all channel members including the kicked user
        std::string kickMsg = ":" + kicker->getNickname() + "!" + kicker->getUsername() + 
                             "@hostname KICK " + _name + " " + user->getNickname();
        if (!reason.empty()) {
            kickMsg += " :" + reason;
        }
        kickMsg += "\r\n";
        
        // Send to all members including the kicked user
        for (std::set<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
            _sendQueue->enqueueMessage((*it)->getFd(), kickMsg);
        }
        _sendQueue->enqueueMessage(user->getFd(), kickMsg);
    }
}

void Channel::inviteUser(User* user) {
    _invited.insert(user);
}

bool Channel::canJoin(User* user, const std::string& key) const {
    // Check if channel is invite-only and user is not invited
    if (_inviteOnly && !isInvited(user)) {
        return false;
    }
    
    // Check if channel has a key and the provided key doesn't match
    if (!_key.empty() && key != _key) {
        return false;
    }
    
    // Check if channel is full
    if (_userLimit > 0 && static_cast<int>(_members.size()) >= _userLimit) {
        return false;
    }
    
    return true;
}

void Channel::broadcastMessage(const std::string& message, User* sender) {
    for (std::set<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != sender) {
            _sendQueue->enqueueMessage((*it)->getFd(), message);
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

const std::string& Channel::getTopicWho() const {
    return _topicWho;
}

time_t Channel::getTopicTime() const {
    return _topicTime;
}

size_t Channel::getMemberCount() const {
    return _members.size();
}

std::string Channel::getMembersList() const {
    std::string membersList;
    for (std::set<User*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (!membersList.empty()) {
            membersList += " ";
        }
        if (isOperator(*it)) {
            membersList += "@";
        }
        membersList += (*it)->getNickname();
    }
    return membersList;
}

std::string Channel::getModeString() const {
    std::string modes = "+";
    if (_inviteOnly) modes += "i";
    if (_topicRestricted) modes += "t";
    if (_moderated) modes += "m";
    if (_noExternalMessages) modes += "n";
    if (_secret) modes += "s";
    if (_private) modes += "p";
    if (!_key.empty()) modes += "k";
    if (_userLimit > 0) modes += "l";
    
    return modes == "+" ? "" : modes;
}

// --- Operator & Member Checks ---
bool Channel::isOperator(User* user) const {
    return _operators.count(user) == 1;
}

bool Channel::isMember(User* user) const {
    return _members.count(user) == 1;
}

bool Channel::isInvited(User* user) const {
    return _invited.count(user) == 1;
}

// --- Mode Management Functions ---
void Channel::setInviteOnly(bool status) {
    _inviteOnly = status;
}

void Channel::setTopicRestricted(bool status) {
    _topicRestricted = status;
}

void Channel::setModerated(bool status) {
    _moderated = status;
}

void Channel::setNoExternalMessages(bool status) {
    _noExternalMessages = status;
}

void Channel::setSecret(bool status) {
    _secret = status;
}

void Channel::setPrivate(bool status) {
    _private = status;
}

void Channel::setKey(const std::string& key) {
    _key = key;
}

void Channel::removeKey() {
    _key = "";
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

void Channel::removeUserLimit() {
    _userLimit = 0;
}

void Channel::addOperator(User* user) {
    if (isMember(user)) {
        _operators.insert(user);
    }
}

void Channel::removeOperator(User* user) {
    _operators.erase(user);
}

void Channel::setTopic(const std::string& topic, User* user) {
    _topic = topic;
    if (user) {
        _topicWho = user->getNickname();
        _topicTime = time(NULL);
    }
}

// --- Getters for Modes & Attributes ---
bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

bool Channel::isModerated() const {
    return _moderated;
}

bool Channel::hasNoExternalMessages() const {
    return _noExternalMessages;
}

bool Channel::isSecret() const {
    return _secret;
}

bool Channel::isPrivate() const {
    return _private;
}

const std::string& Channel::getKey() const {
    return _key;
}

bool Channel::hasKey() const {
    return !_key.empty();
}

int Channel::getUserLimit() const {
    return _userLimit;
}

bool Channel::hasUserLimit() const {
    return _userLimit > 0;
}