#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel {
private:
    std::string         _name;
    std::string         _topic;
    std::string         _key;

    std::set<User*>     _members;
    std::set<User*>     _operators;

    bool                _inviteOnly;
    bool                _topicRestricted;
    int                 _userLimit;

public:
    // --- Constructor & Destructor ---
    Channel(const std::string& name);
    ~Channel();

    // --- User Management ---
    void                addUser(User* user);
    void                removeUser(User* user);
    void                broadcastMessage(const std::string& message, User* sender);

    // --- Getters ---
    const std::string&  getName() const;
    const std::string&  getTopic() const;
    size_t              getMemberCount() const;

    // --- Operator & Member Checks ---
    bool                isOperator(User* user) const;
    bool                isMember(User* user) const;

    // --- Mode Management Functions ---
    void                setInviteOnly(bool status);
    void                setTopicRestricted(bool status);
    void                setKey(const std::string& key);
    void                setUserLimit(int limit);
    void                addOperator(User* user);
    void                removeOperator(User* user);
    void                setTopic(const std::string& topic);

    // --- Getters for Modes & Attributes ---
    bool                isInviteOnly() const;
    const std::string&  getKey() const;
    int                 getUserLimit() const;
};

#endif