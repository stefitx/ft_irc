#include "../../inc/Server.hpp"

int Server::killCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return (461); // ERR_NEEDMOREPARAMS
	if (!client.getServerOper())
		return (481); // ERR_NOPRIVILEGES
	std::string targetNick = args[0];
	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second->getNick() == targetNick)
		{
			Client *targetClient = it->second;
			if (targetClient->getFd() == client.getFd())
				return (442); // ERR_NOTONCHANNEL
			std::string killMessage = args[1];
			std::string killLine = ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " KILL " + targetNick + killMessage;
			sendLine(*targetClient, killLine + "\r\n");
			std::map<std::string, Channel *>::iterator it;
			for (it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
				Channel* chan = getChannel(it->first);
				if (chan) {
					noticeCmd(client, vectorSplit(chan->getName() + " KILLER " + client.getNick() + " killed " + targetNick + " and gave this reason" + killMessage, ' '));
					chan->broadcast(killLine, client);
					chan->removeMember(&client);
				}
			}
			disconnectClient(*targetClient);
			return (0);
		}
	}
	return (401); // ERR_NOSUCHNICK
}
