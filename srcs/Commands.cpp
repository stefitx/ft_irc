#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

void	Server::executeCmd(Client &client, std::string cmd, std::vector<std::string> args)
{
	int code = 0;
	args.erase(args.begin());
	// std::cout << "nick = " << client.getNick() << std::endl;
	// std::cout << "pass = " << client.getRegistryState() << std::endl;
	// std::cout << "user = " << client.getUser() << std::endl;

	if(isCommand(cmd) == UNKNOWN && cmd != "CAP" && cmd != "WHO")
		return (errorReply(client, 421, cmd, args), void());
	else if (!client.getHandShake() && isCommand(cmd) != PASS && isCommand(cmd) != NICK && isCommand(cmd) != USER && cmd != "CAP" && cmd != "WHO")
		return(errorReply(client, 451, cmd, args), void());
	if(isCommand(cmd) == NICK || isCommand(cmd) == USER || isCommand(cmd) == PASS)
	{
		if (isCommand(cmd) == NICK)
			code = nickCmd(client, args);
		else if (isCommand(cmd) == USER)
			code = userCmd(client, args);
		else if (isCommand(cmd) == PASS)
			code = passCmd(client, args);
		if((client.getRegistryState() && client.getNick() != "" && client.getUser() != "") && !client.getHandShake())
			handshake(client);
		std::cout << RED << "[" << code << "] " << RESET << std::endl;
		if (code)
			errorReply(client, code, cmd, args);
		return ;
	}
	switch (isCommand(cmd))
	{
		case NICK: code = nickCmd(client, args); break;
		case USER: code = userCmd(client, args); break;
		case PASS: code = passCmd(client, args); break;
		case JOIN: code = joinCmd(client, args); break;
		case PART: break;//code = partCmd(client, args); break;
		case TOPIC: break;// code = topic(client, args); break;
		case INVITE: break;// code = invite(client, args); break;
		case KICK: break;// code = kick(client, args); break;
		case MODE: break;// code =mode(client, args); break;
		case OPER: code = operCmd(client, args); break;
		case PRIVMSG: code = privmsgCmd(client, args); break;
		case HELP: code = helpCmd(client, args); break;
		case DIE: code = dieCmd(client, args); break;
		case QUIT: code = quitCmd(client, args); break;
		case UNKNOWN:
		{
			if(cmd == "WHO" || cmd == "CAP")
				return;
				// ERR_UNKNOWNCOMMAND (421) (send to client)
			code = 421;
			std::cout << cmd << ": Unknow command\n";
		}
	}
	if (code)
		errorReply(client, code, cmd, args);
}

int    Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
		return 461;
		// return reply(client, 461, "" , "] NICK: not enough params") ? 0 : -1;
	std::map<int, Client*>::iterator	it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == args[0])
			return 433;
			// return reply(client, 433, args[0], "] NICK: sorry, nick already in use!") ? 0 : -1;
		it++;
	}
	std::cout << "Nick changed from " << client.getNick() << " to " << args[0] << "\n";
    client.setNick(args[0]);
	std::cout << "Registry state: " << client.getRegistryState() << ", Handshake: " << client.getHandShake() << "\n";

	return (0);
}

int Server::passCmd(Client &client, std::vector<std::string> args)
{
	if (client.getRegistryState() == true)
		return 462;
		// return reply(client, 462, "", "] PASS: stop it, you've already registered") ? 0 : -1;
	else if (args.empty())
		return 461;
		// return reply(client, 461, "", "] PASS: empty password!") ? 0 : -1;
	else if (args[0] != _password)
		return 464;
		// return reply(client, 464, "", "] PASS: wrong password!") ? 0 : -1;
	client.setRegistryState(true);
	std::cout << "Client fd " << client.getFd() << " authenticated successfully :)\n";
	return (0);

}

int Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (client.getHandShake() == true)
		return 462;
	// return reply(client, 462, args[0], "] USER: already registered") ? 0 : -1;

	if (args.size() < 4)
		return 461;
		// return reply(client, 461, "", "] USER: Not enough params") ? 0 : -1;
	client.setUser(args[0]);
	client.setHost(inet_ntoa(client.getAddr().sin_addr));
	std::cout << "Client fd " << client.getFd() << " ip: " << client.getIp() << std::endl;
	return (0);
}

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

Channel	*Server::getChannel(const std::string	&name)
{
	std::map<std::string, Channel *>::iterator	it;

	it = _channels.find(name);
	if (it == _channels.end())
		return (NULL);
	return (it->second);
}

void	Server::createChannel(std::string channelName, std::string key, Client *client)
{
	Channel *newChannel = new Channel(channelName);

	if (!key.empty())
		newChannel->setPassword(key);
	newChannel->addMember(client);
	newChannel->addOperator(client);
	newChannel->setChanOperator(client);

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
			{
				_channels[channel]->addMember(&client);
			}
			else
			{
				//mirar esos codigos de ERR
				//reply(authCode);
			}
		}
		joins_it++;
	}
	delete joins;
	return (0);
}

int Server::helpCmd(Client &client, std::vector<std::string> args)
{
	if(args.empty())
	{
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 704 " + client.getNick() + " index " + ": Commands Available:\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": NICK\t\tUSER\t\tPASS   \t\tQUIT\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": JOIN\t\tPART\t\tTOPIC  \t\tINVITE\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": KICK\t\tMODE\t\tPRIVMSG\t\tOPER\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 706 " + client.getNick() + " index " + ": Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
	}
	else if (args[0] == "-l")
	{
		sendLine(client, "\r\n");
		sendLine(client, " Commands Available:\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t NICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: NICK <nickname>, sets your nick\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t USER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: USER <username> <hostname> <servername> :<realname>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PASS" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PASS <password>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t QUIT" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: QUIT [<reason>], disconnects from the current server\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t JOIN" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: JOIN <channel>, joins the channel\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PART" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t TOPIC" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t INVITE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t KICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: KICK <nick> [reason], kicks the nick from the current channel (needs chanop)\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t MODE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: MODE <channel> [<mode>]\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PRIVMSG" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PRIVMSG <target> :<message>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t OPER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: OPER <username> <password>, grants operator privileges\r\n");
		sendLine(client, "\r\n");
		sendLine(client, " Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
		/* ---------------------------------------------------------- */
		// sendLine(client, "\r\n");
		// sendLine(client, ":" + _hostname + " 704 " + client.getNick() + " index " + ": Commands Available:\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tNICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: NICK <nickname>, sets your nick\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tUSER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: USER <username> <hostname> <servername> :<realname>\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tPASS" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PASS <password>\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tQUIT" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: QUIT [<reason>], disconnects from the current server\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tJOIN" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: JOIN <channel>, joins the channel\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tPART" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tTOPIC" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tINVITE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tKICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: KICK <nick> [reason], kicks the nick from the current channel (needs chanop)\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tMODE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: MODE <channel> [<mode>]\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tPRIVMSG" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PRIVMSG <target> :<message>\r\n");
		// sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": " + (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\tOPER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: OPER <username> <password>, grants operator privileges\r\n");
		// sendLine(client, "\r\n");
		// sendLine(client, ":" + _hostname + " 706 " + client.getNick() + " index " + ": Type /WELP <command> for more information, or /WELP -l\r\n");
		// sendLine(client, "\r\n");
	}
	else
	{
		sendLine(client, "\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + " Usage\r\n" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));

		if (args[0] == "NICK" || args[0] == "nick")
			sendLine(client, "\t\tNICK <nickname>, sets your nick\r\n");
		else if (args[0] == "USER" || args[0] == "user")
			sendLine(client, "\t\tUSER <username> <hostname> <servername> :<realname>\r\n");
		else if (args[0] == "PASS" || args[0] == "pass")
			sendLine(client, "\t\tPASS <password>\r\n");
		else if (args[0] == "QUIT" || args[0] == "quit")
			sendLine(client, "\t\tQUIT [<reason>], disconnects from the current server\r\n");
		else if (args[0] == "JOIN" || args[0] == "join")
			sendLine(client, "\t\tJOIN <channel>, joins the channel\r\n");
		else if (args[0] == "PART" || args[0] == "part")
			sendLine(client, "\t\tPART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		else if (args[0] == "TOPIC" || args[0] == "topic")
			sendLine(client, "\t\tTOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		else if (args[0] == "INVITE" || args[0] == "invite")
			sendLine(client, "\t\tINVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		else if (args[0] == "KICK" || args[0] == "kick")
			sendLine(client, "\t\tKICK <nick> [reason], kicks the nick from the current channel (needs chanop)\r\n");
		else if (args[0] == "MODE" || args[0] == "mode")
			sendLine(client, "\t\tMODE <channel> [<mode>]\r\n");
		else if (args[0] == "PRIVMSG" || args[0] == "privmsg")
			sendLine(client, "\t\tPRIVMSG <target> :<message>\r\n");
		else if (args[0] == "OPER" || args[0] == "oper")
			sendLine(client, "\t\tOPER <username> <password>, grants operator privileges\r\n");
		else
		{
			// ERR_HELPNOTFOUND (524) -> "client> <subject> :No help available on this topic"
			std::cerr << "[" << client.getFd() << "] HELP: No help available on this topic\n";
			return (524);
		}
	}
	return (0);
}

int Server::operCmd(Client &client, std::vector<std::string> args)
{
	//should we cover ERR_NOOPERHOST (491)? (check the hostname/IP of the client)
	if (args.size() < 2)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] OPER: Not enough params\n";
		return (461);
	}
	std::map<std::string, std::string>::iterator	it;
	for(it = _operator_credentials.begin(); it != _operator_credentials.end();)
	{
		if (it->first == args[0] && it->second == args[1])
			break;
		it++;
	}
	if (it == _operator_credentials.end())
	{
		// ERR_PASSWDMISMATCH (464) -> "<client> :Password incorrect"
		std::cerr << "[" << client.getFd() << "] OPER: wrong credentials...\n";
		return (464);
	}
	client.setNick(args[0]);
	client.setUser(args[0]);
	client.setServerOper(true);
	// RPL_YOUREOPER (381)
	reply(client, 381, "", (client.getIsNetCat() ? std::string(GREEN) : std::string("\00303")) + "You are now an IRC operator" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));
	return (0);
}

int Server::dieCmd(Client &client, std::vector<std::string> args)
{
	(void)args;
	if (client.getServerOper() == false)
	{
		// ERR_NOPRIVILEGES (481) -> "<client> :Permission Denied- You're not an IRC operator"
		std::cerr << "[" << client.getFd() << "] DIE: You are not an operator!\n";
		return (481);
	}
	std::map<int, Client *>::iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		reply(*it->second, 400, "", "Server is shutting down. Goodbye!");
		std::cout << "Notifying client fd " << it->second->getFd() << " about server shutdown...\n";
	}
	exit(0);
	return (0);
}

int Server::quitCmd(Client &client, std::vector<std::string> args)
{
	std::map<std::string, Channel *>::iterator it;
	for (it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
        Channel* chan = getChannel(it->first);
        if (chan) {
			std::string quit_line = ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " QUIT " + (args.empty() ? "Client Quit" : args[0]);
			chan->broadcast(quit_line, client);
            chan->removeMember(&client);
        }
    }
	disconnectClient(client);
	return (0);
}

void Server::disconnectClient(Client &client)
{
	int fd = client.getFd();
	std::cout << "Disconnecting client fd " << fd << "...\n";

	std::map<std::string, Channel *>& channels = client.getChannels();
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* chan = getChannel(it->first);
		if (chan) {
			chan->removeMember(&client);
			// hacer broadcast a los miembros del canal
			reply(client, 404, it->first, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "Disconnected()" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));
		}
	}
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == fd) {
			std::cout << "Removing client fd " << fd << " from pollFds...\n";
			removeClient(i);
			break;
		}
	}
	std::cout << "Client fd " << fd << " disconnected successfully.\n";
}

CommandType Server::isCommand(const std::string &cmd)
{
	if (cmd == "PASS") return (PASS);
	else if (cmd == "NICK") return (NICK);
	else if (cmd == "USER") return (USER);
	else if (cmd == "QUIT") return (QUIT);
	else if (cmd == "HELP" || cmd == "WELP" || cmd == "welp") return (HELP);
	else if (cmd == "JOIN") return (JOIN);
	else if (cmd == "PART") return (PART);
	else if (cmd == "TOPIC") return (TOPIC);
	else if (cmd == "INVITE") return (INVITE);
	else if (cmd == "KICK") return (KICK);
	else if (cmd == "MODE") return (MODE);
	else if (cmd == "OPER" || cmd == "oper") return (OPER);
	else if (cmd == "PRIVMSG" || cmd == "privmsg")return (PRIVMSG);
	else if (cmd == "DIE") return (DIE);
	// else if (cmd == "CAP") return (CAP);
	else
		return (UNKNOWN);
}

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
