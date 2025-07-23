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
			reply(*targetClient, 403, targetNick, killLine);
			//broadcast to all clients -> to be continued
			disconnectClient(*targetClient);
			return (0);
		}
	}
	return (401); // ERR_NOSUCHNICK
}
