#include "../../inc/Server.hpp"

int Server::partCmd(Client &client, std::vector<std::string> args)
{
	if (!client.getRegistryState())
		return (451);
	if (args.empty())
		return (461);

	std::vector<std::string> channels = vectorSplit(args[0], ',');
	std::string reason;
	if (args[0] == "0") // prepare to part from all the channels the client is member of
	{
		std::map<std::string, Channel *>::iterator clientChannels_it = client.getChannels().begin();
		channels.clear();
		std::vector<std::string>::iterator channelsVector_it = channels.begin();
		for (; clientChannels_it != client.getChannels().end(); clientChannels_it++)
		{
			channels.insert(channelsVector_it, clientChannels_it->first);
			channelsVector_it++;
		}
		reason = "";
	}
	else if (args.size() > 1)
	{
		reason = args[1];
		for (size_t i = 2; i < args.size(); ++i)
			reason += " " + args[i];
		if (!reason.empty() && reason[0] == ':')
			reason.erase(0, 1);
	}
	for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		const std::string &chanName = *it;
		Channel *chan = getChannel(chanName);

		if (!chan)
		{
			std::vector<std::string> singleArg(1, chanName);
			errorReply(client, 403, chanName, singleArg);
			continue;
		}
		if (!chan->isMember(&client))
		{
			std::vector<std::string> singleArg(1, chanName);
			errorReply(client, 442, chanName, singleArg);
			continue;
		}
		std::string partLine = ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " PART " + chanName;
		if (!reason.empty())
			partLine += " :" + reason;

		chan->broadcast(partLine, client);
		sendLine(client, partLine + "\r\n");

		chan->removeMember(&client);
		chan->removeOperator(&client);
		client.removeJoinedChannel(chan);
		pruneChannel(chan);
	}
	return (0);
}
