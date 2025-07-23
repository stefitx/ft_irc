#include "../../inc/Server.hpp"

int Server::inviteCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return (461); // ERR_NEEDMOREPARAMS

	std::string nick = args[0];
	std::string channel = args[1];

	if (!getChannel(channel) || channel[0] != '#')
		return (403); // ERR_NOSUCHCHANNEL

	Channel *chan = getChannel(channel);

	if (!chan->isMember(&client))
		return (442); // ERR_NOTONCHANNEL
	if (chan->isInviteMode() && !chan->isOperator(&client))
		return (482); // ERR_CHANOPRIVSNEEDED
	if (chan->getMembers(nick))
		return (443); // ERR_USERONCHANNEL

	Client *targetClient;
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNick() == nick)
			targetClient = it->second;
	}
	chan->addIvitedUser(targetClient);
	sendLine(client, ":" + _hostname + " 341 " + client.getNick() + " " + targetClient->getNick() + " " + channel + "\r\n");
	sendLine(*targetClient, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " INVITE " + targetClient->getNick() + " :" + channel + "\r\n");
	return (0);
}
