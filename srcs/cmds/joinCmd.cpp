#include "../../inc/Server.hpp"

std::vector<std::string> Server::vectorSplit(const std::string& str, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);

    while (std::getline(ss, token, delim))
	{
        tokens.push_back(token);
    }
    return (tokens);
}

void	Server::createChannel(std::string channelName, std::string key, Client *client)
{
	Channel *newChannel = new Channel(channelName);

	if (!key.empty())
		newChannel->setPassword(key);
	newChannel->addMember(client);
	newChannel->addOperator(client);
	newChannel->setMode("Cnst");

	_channels[channelName] = newChannel;
}

std::map<std::string, std::string>	*Server::parseJoinArgs(std::vector<std::string> args)
{
	std::map<std::string, std::string>	*joins;
	joins = new std::map<std::string, std::string>;

	std::vector<std::string> channels = vectorSplit(args[0], ',');
    std::vector<std::string> keys;
    if (args.size() > 1)
		keys = vectorSplit(args[1], ',');
	std::string	channelKey;
    for (size_t i = 0; i < channels.size(); ++i)
	{
		if (i < keys.size())
			channelKey = keys[i];
		else
			channelKey = "";
		(*joins)[channels[i]] = channelKey;
	}
	return (joins);
}

int	Server::joinCmd(Client &client, std::vector<std::string> args)
{
	std::map<std::string, std::string>	*joins;
	if (!client.getRegistryState())
	{
		//ERR_NOTREGISTERED (451)
		return (451);
	}
	if (!args.size())
		return 461;
	if (args.size() == 1 && args[0] == "0")
	{
		//partCmd(client, NULL);
		return (0);
	}
	joins = parseJoinArgs(args);
	if (!joins)
		return (0);

	std::map<std::string, std::string>::iterator	joins_it = joins->begin();
	while (joins_it != joins->end())
	{
		std::string	channel = joins_it->first;
		std::string	key = joins_it->second;
		if (!getChannel(joins_it->first)) // si no encuentras el canal
		{
			if (channel[0] != '#')
			{
				sendLine(client, ":" + _hostname + " " + itoa3(403) + " " + (client.getNick().empty() ? "*" : client.getNick()) + " " + channel + " :No such channel\r\n"); //ERR 403
				++joins_it;
				continue;
			}
			else
			{
				createChannel(channel, key, &client);
				client.addJoinedChannel(getChannel(channel));
				getChannel(channel)->addOperator(&client);

				sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + "\r\n");
				sendLine(client, ":" + _hostname + " 353 " + client.getNick() + " @ " + channel + " :@" + client.getNick() + "\r\n");
				sendLine(client, ":" + _hostname + " 366 " + client.getNick() + " " + channel + " :" + "End of /NAMES list\r\n");
			}

		}
		else // el channel ya exise
		{
			if (client.getChannelsJoined() <= CHANLIMITNUM - 1)
			{
				int authCode = getChannel(channel)->authorizedToJoin(&client, key);
				if (authCode == 0)
					_channels[channel]->addMember(&client);	// add client to channel
				else
				{
					errorReply(client, authCode, channel, args);
					++joins_it;
					continue;
				}
				sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + "\r\n");
				std::map<std::string, Client *>& members = _channels[channel]->getMapMembers();
				std::string names_list;
				for (std::map<std::string, Client *>::iterator it = members.begin(); it != members.end(); ++it)
				{
					Client* member = it->second;
					std::string member_nick = member->getNick();
					if (!names_list.empty())
					{
						names_list += " ";
						if (getChannel(channel)->isChannelOperator(member_nick))
							names_list += "@";
					}
					names_list += member_nick;
				}
				if (!getChannel(channel)->getTopic().empty())
				{
					// SEND RPL_TOPIC 332
					sendLine(client, ":" + _hostname + " 332 " + client.getNick() + channel + " :" + getChannel(channel)->getTopic() + "\r\n");
					// SEND RPL_TOPICWHOTIME 333
				}
				sendLine(client, ":" + _hostname + " 353 " + client.getNick() + " @ " + channel + " :"  + names_list + "\r\n");
				sendLine(client, ":" + _hostname + " 366 " + client.getNick() + " " + channel + " :" + "End of /NAMES list\r\n");
				getChannel(channel)->broadcast(":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + " :realname", client);
				client.addJoinedChannel(getChannel(channel));
			}
			else
			{
				delete joins;
				return (405); // ERR_TOOMANYCHANNELS (405)
			}
		}
		joins_it++;
	}
	delete joins;
	return (0);
}
