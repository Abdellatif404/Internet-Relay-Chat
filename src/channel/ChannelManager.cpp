#include "ChannelManager.hpp"

ChannelManager::ChannelManager(SendQueue *sendQueue)
	: _sendQueue(sendQueue) {}

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
    Channel* newChannel = new Channel(name, _sendQueue);
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

void ChannelManager::addUserToChannel(User* user, const std::string& channelName, const std::string& key) {
    Channel* channel = getChannel(channelName);
    if (channel == NULL) {
        if (!isValidChannelName(channelName)) {
            return;
        }
        channel = createChannel(channelName);
    }
    if (channel != NULL && channel->canJoin(user, key)) {
        channel->addUser(user);
    }
}

void ChannelManager::removeUserFromChannel(User* user, const std::string& channelName) {
    Channel* channel = getChannel(channelName);
    if (channel != NULL) {
        channel->removeUser(user);
        if (channel->getMemberCount() == 0) {
            deleteChannel(channelName);
        }
    }
}

void ChannelManager::removeUserFromAllChannels(User* user) {
    std::vector<std::string> channelsToCleanup;
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(user)) {
            it->second->removeUser(user);
            if (it->second->getMemberCount() == 0) {
                channelsToCleanup.push_back(it->first);
            }
        }
    }
    
    // Cleanup empty channels
    for (std::vector<std::string>::iterator it = channelsToCleanup.begin(); it != channelsToCleanup.end(); ++it) {
        deleteChannel(*it);
    }
}

// --- Channel Utilities ---
std::vector<std::string> ChannelManager::getUserChannels(User* user) const {
    std::vector<std::string> userChannels;
    
    for (std::map<std::string, Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->isMember(user)) {
            userChannels.push_back(it->first);
        }
    }
    
    return userChannels;
}

bool ChannelManager::isValidChannelName(const std::string& name) const {
    if (name.empty() || name.length() > 50) {
        return false;
    }
    
    // Channel names must start with # or &
    if (name[0] != '#' && name[0] != '&') {
        return false;
    }
    
    // Check for invalid characters
    for (size_t i = 1; i < name.length(); ++i) {
        char c = name[i];
        if (c == ' ' || c == ',' || c == '\007') { // \007 is BEL character
            return false;
        }
    }
    
    return true;
}

void ChannelManager::cleanupEmptyChannels() {
    std::vector<std::string> channelsToDelete;
    
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        if (it->second->getMemberCount() == 0) {
            channelsToDelete.push_back(it->first);
        }
    }
    
    for (std::vector<std::string>::iterator it = channelsToDelete.begin(); it != channelsToDelete.end(); ++it) {
        deleteChannel(*it);
    }
}