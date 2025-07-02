void JoinCommand::execute(User* user, const std::vector<std::string>& params) {
	if (!user->isRegistered()) {
		user->send(ERR_NOTREGISTERED());
		return;
	}
	if (params.empty()) {
		user->send(ERR_NEEDMOREPARAMS("JOIN"));
		return;
	}

	std::string channelName = params[0];
	if (channelName[0] != '#') {
		user->send(ERR_NOSUCHCHANNEL(channelName));
		return;
	}

	Channel* channel = channelManager.getOrCreate(channelName);
	channel->addMember(user);

	// Automatically make first user an operator
	if (channel->getMemberCount() == 1)
		channel->addOperator(user);

	std::string joinMsg = ":" + user->getPrefix() + " JOIN " + channelName;
	channel->broadcast(joinMsg, NULL);

}
