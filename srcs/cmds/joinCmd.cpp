#include "../../inc/Server.hpp"

std::vector<std::string> vectorSplit(const std::string& str, char delim)
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
	newChannel->setChanOperator(client);
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
		//joins[channels[i]].insert(channelKey);
		std::cout << "channel " << channels[i] << ", key: " << channelKey << std::endl;
	}
	return (joins);
}

int	Server::joinCmd(Client &client, std::vector<std::string> args)
{
	std::cout << "ip : " << client.getIp() << std::endl;
	std::map<std::string, std::string>	*joins;

	// std::cout << "heyy im in joinn\n";

	if (!client.getRegistryState())
	{
		//ERR_NOTREGISTERED (451)
		std::cout << "You haven't registered yet!\n";
		return (451);
	}
	if (!args.size())
		return 461;
	if (args.size() == 1 && args[0] == "0")
	{
		//partCmd(client, NULL);
		std::cout << "should be parting from all channels he is in\n";
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
				//ERR 403 -> "there is no such channel"
				return (403);
			}
			else
			{
				createChannel(channel, key, &client);
				client.addJoinedChannel(getChannel(channel));
				// std::string	arg = "JOIN " + channel;
				// std::string arg = ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname;
				sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + "\r\n");

				// sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + " * :" + "Welcome to the channel!\r\n");
				sendLine(client, ":" + _hostname + " 353 " + client.getNick() + " @ " + channel + " :@" + client.getNick() + "\r\n");
				sendLine(client, ":" + _hostname + " 366 " + client.getNick() + " " + channel + " :" + "End of /NAMES list\r\n");

				// RPL_NAMEREPLY 353
				// send RPL_ENDOFNAMES 366
				// reply(client, 0, "", );
			}

		}
		else // el channel ya exise
		{
			if ( client.getChannelsJoined() <= CHANLIMIT - 1)
			{
				// add client to channel
				getChannel(channel)->addMember(&client);
				sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + "\r\n");

				// sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname + " JOIN " + channel + " * :" + "Welcome to the channel!\r\n");
				std::map<std::string, Client *>& members = _channels[channel]->getMapMembers();
				std::string names_list;
				for (std::map<std::string, Client *>::iterator it = members.begin(); it != members.end(); ++it)
				{
					Client* member = it->second; // pointer to the Client object
					std::string member_nick = member->getNick();
					if (!names_list.empty())
						names_list += " ";
					names_list += member_nick;
					// Now you can use member_nick in your reply
					// Example: sendLine(client, ":" + member_nick + "!" + ... );
				}
				if (!getChannel(channel)->getTopic().empty())
				{
					// SEND :molybdenum.libera.chat 332 martaa #holiboli :iwantanothertopic (RPL_TOPIC)
					// SEND 333 (RPL_TOPICWHOTIME)
				}
				sendLine(client, ":" + _hostname + " 353 " + client.getNick() + " @ " + channel + " :@"  + names_list + "\r\n");
				sendLine(client, ":" + _hostname + " 366 " + client.getNick() + " " + channel + " :" + "End of /NAMES list\r\n");
				getChannel(channel)->broadcast(":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " JOIN " + channel + " :realname", client);
				client.addJoinedChannel(getChannel(channel));
				// for (std::map<std::string, Client *>::iterator members_it = _channels[channel]->getMapMembers().begin(); members_it != _channels[channel]->getMapMembers().end(); members_it++)
				// {
				// 	std::map<std::string, Client *> mapa = _channels[channel]->getMapMembers();
				// 	std::string member_name = getChannel(channel);
				// 	sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname + );

				// }
			}
			else
			{
				// ERR_TOOMANYCHANNELS (405)
				return (405);
			}
			int authCode = getChannel(channel)->authorizedToJoin(&client, key);
			if (authCode == 0) // client is authorized to join -> cumplen con: key, client limit , ban - exception, invite-only - exception
				_channels[channel]->addMember(&client);
			// errorReply(authcode);

		}
		joins_it++;
	}
	delete joins;
	return (0);
}
