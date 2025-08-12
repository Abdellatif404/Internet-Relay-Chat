#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"
#include "User.hpp"
#include "SendQueue.hpp"

class Channel {
private:
    std::string         _name;
    std::string         _topic;
    std::string         _key;
    std::string         _topicWho;
    time_t              _topicTime;

    std::set<User*>     _members;
    std::set<User*>     _operators;
    std::set<User*>     _invited;

    bool                _inviteOnly;
    bool                _topicRestricted;
    bool                _moderated;
    bool                _noExternalMessages;
    bool                _secret;
    bool                _private;
    int                 _userLimit;

	SendQueue*			_sendQueue;

public:
    // --- Constructor & Destructor ---
    Channel(const std::string& name, SendQueue* sendQueue);
    ~Channel();

    // --- User Management ---
    void                addUser(User* user);
    void                removeUser(User* user);
    void                kickUser(User* user, User* kicker, const std::string& reason);
    void                inviteUser(User* user);
    bool                canJoin(User* user, const std::string& key) const;
    void                broadcastMessage(const std::string& message, User* sender);

    // --- Getters ---
    const std::string&  getName() const;
    const std::string&  getTopic() const;
    const std::string&  getTopicWho() const;
    time_t              getTopicTime() const;
    size_t              getMemberCount() const;
    std::string         getMembersList() const;
    std::string         getModeString() const;

    // --- Operator & Member Checks ---
    bool                isOperator(User* user) const;
    bool                isMember(User* user) const;
    bool                isInvited(User* user) const;

    // --- Mode Management Functions ---
    void                setInviteOnly(bool status);
    void                setTopicRestricted(bool status);
    void                setModerated(bool status);
    void                setNoExternalMessages(bool status);
    void                setSecret(bool status);
    void                setPrivate(bool status);
    void                setKey(const std::string& key);
    void                removeKey();
    void                setUserLimit(int limit);
    void                removeUserLimit();
    void                addOperator(User* user);
    void                removeOperator(User* user);
    void                setTopic(const std::string& topic, User* user);

    // --- Getters for Modes & Attributes ---
    bool                isInviteOnly() const;
    bool                isTopicRestricted() const;
    bool                isModerated() const;
    bool                hasNoExternalMessages() const;
    bool                isSecret() const;
    bool                isPrivate() const;
    const std::string&  getKey() const;
    bool                hasKey() const;
    int                 getUserLimit() const;
    bool                hasUserLimit() const;
};

#endif