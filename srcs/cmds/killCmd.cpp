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
			sendLine(*targetClient, killLine + "\r\n");

			disconnectClient(*targetClient, "Killed by " + client.getNick() + " (" + message + ")");
			return (0);
		}
	}
	return (errorReply(client, 401, targetNick, args), 0); // ERR_NOSUCHNICK
}
