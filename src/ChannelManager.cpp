#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {
}

ChannelManager::~ChannelManager() {
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    _channels.clear();
}

// --- Channel Management ---
Channel* ChannelManager::createChannel(const std::string& name) {
    if (getChannel(name) != NULL) {
        return NULL;
    }
    Channel* newChannel = new Channel(name);
    _channels[name] = newChannel;
    return newChannel;
}

Channel* ChannelManager::getChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

void ChannelManager::deleteChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        delete it->second;
        _channels.erase(it);
    }
}

void ChannelManager::addUserToChannel(User* user, const std::string& channelName) {
    Channel* channel = getChannel(channelName);
    if (channel == NULL) {
        channel = createChannel(channelName);
    }
    if (channel != NULL) {
        channel->addUser(user);
    }
}