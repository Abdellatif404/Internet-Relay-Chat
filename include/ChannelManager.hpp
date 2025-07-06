#ifndef CHANNEL_MANAGER_HPP
#define CHANNEL_MANAGER_HPP

#include "Channel.hpp"

class ChannelManager {
private:
    std::map<std::string, Channel*> _channels;

public:
    // --- Constructor & Destructor ---
    ChannelManager();
    ~ChannelManager();

    // --- Channel Management ---
    Channel* createChannel(const std::string& name);
    Channel* getChannel(const std::string& name);
    void deleteChannel(const std::string& name);
    void addUserToChannel(User* user, const std::string& channelName);
};

#endif