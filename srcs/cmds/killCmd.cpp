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
			std::string message;
			for (size_t i = 1; i < args.size(); ++i)
			{
				if (i > 1)
					message += " ";
				message += args[i];
			}
			if (!message.empty() && message[0] == ':')
				message.erase(0, 1);
			std::string killLine = ":" + targetClient->getNick() + "!~" + targetClient->getUser() + "@" + targetClient->getIp() + " KILL " + targetNick  + " :" + message;
			std::string quitMsg = ":" + targetClient->getNick() + "!~" + targetClient->getUser() + "@" + targetClient->getIp() + " QUIT :Killed by " + client.getNick() + " (" + message + ")";
			sendLine(*targetClient, killLine + "\r\n");
			std::map<std::string, Channel *>::iterator it;
			for (it = targetClient->getChannels().begin(); it != targetClient->getChannels().end(); ++it)
			{
				Channel* chan = getChannel(it->first);
				if (chan) {
					chan->broadcast(quitMsg, *targetClient);
					chan->removeMember(targetClient);
				}
			}
			disconnectClient(*targetClient);
			return (0);
		}
	}
	return (401); // ERR_NOSUCHNICK
}
