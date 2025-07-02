#ifndef CHANNEL_MANAGER_HPP
#define CHANNEL_MANAGER_HPP

#include <map>
#include <string>
#include "Channel.hpp"

class ChannelManager {
private:
	std::map<std::string, Channel*> channels;

public:
	ChannelManager();
	~ChannelManager();

	Channel* getOrCreate(const std::string& name);
	Channel* getChannel(const std::string& name);
	void removeIfEmpty(Channel* channel);
};

#endif
