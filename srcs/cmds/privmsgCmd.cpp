#include "../../inc/Server.hpp"

int Server::privmsgCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
	{
		// ERR_NOTEXTTOSEND (411) -> "<client> :No recipient given (<command>)"
		std::cerr << "[" << client.getFd() << "] PRIVMSG: No text to send\n";
		return (411);
	}
	else if (args.size() < 2)
	{
		// ERR_NOTEXTTOSEND (412) -> "<client> :No text to send"
		std::cerr << "[" << client.getFd() << "] PRIVMSG: No text to send\n";
		return (412);
	}
	// std::string target = args[0];
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
	std::cout << "PRIVMSG: target: " << target << ", message: " << message << std::endl;
	for (size_t i = 0; i < targets.size(); ++i)
	{
		std::cout << "PRIVMSG: target[" << i << "]: " << targets[i] << std::endl;
		std::string &tgt = targets[i];
		if (tgt[0] == '#')
		{
			Channel *channel = getChannel(tgt);
			if (!channel)
			{
				std::cerr << "[" << client.getFd() << "] PRIVMSG: No such nick/channel: " << tgt << std::endl;
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
