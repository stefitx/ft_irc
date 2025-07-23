#include "../../inc/Server.hpp"

int Server::privmsgCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
		return (411); // ERR_NOTEXTTOSEND (411)
	else if (args.size() < 2)
		return (412); // ERR_NOTEXTTOSEND (412)

	std::vector<std::string> targets;
	std::stringstream ss(args[0]);
	std::string target;

	while (std::getline(ss, target, ','))
	{
		if (!target.empty())
			targets.push_back(target);
	}

	std::string message;
	for (size_t i = 1; i < args.size(); ++i)
	{
		if (i > 1)
			message += " ";
		message += args[i];
	}
	if (!message.empty() && message[0] == ':')
		message.erase(0, 1);
	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::string &tgt = targets[i];
		if (tgt[0] == '#')
		{
			Channel *channel = getChannel(tgt);
			if (!channel)
			{
				errorReply(client, 401, tgt, args);
				continue;
			}
			channel->broadcast(":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " PRIVMSG " + tgt + " :" + message, client);
		}
		else
		{
			bool found = false;
			for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				if (it->second->getNick() == tgt)
				{
					sendLine(*it->second, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " PRIVMSG " + tgt + " :" + message + "\r\n");
					found = true;
					break;
				}
			}
			if (!found)
				errorReply(client, 401, tgt, args);
		}
	}
	return 0;
}
