#include "ChannelManager.hpp"

ChannelManager::ChannelManager() {}

ChannelManager::~ChannelManager() {
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		delete it->second;
}

Channel* ChannelManager::getOrCreate(const std::string& name) {
	if (channels.find(name) == channels.end())
		channels[name] = new Channel(name);
	return channels[name];
}

Channel* ChannelManager::getChannel(const std::string& name) {
	if (channels.find(name) != channels.end())
		return channels[name];
	return NULL;
}

void ChannelManager::removeIfEmpty(Channel* channel) {
	if (channel && channel->getMemberCount() == 0)
		channels.erase(channel->getName());
}
