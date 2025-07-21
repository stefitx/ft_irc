#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

void	Server::executeCmd(Client &client, std::string cmd, std::vector<std::string> args)
{
	int code = 0;
	args.erase(args.begin());
	if (!client.getHandShake() && cmd != "PASS" && cmd != "NICK" && cmd != "USER")
	{
		errorReply(client, 451, cmd);
		return;
	}
	if(cmd == "NICK" || cmd == "USER" || cmd == "PASS")
	{
		if (cmd == "NICK")
			code = nickCmd(client, args);
		else if (cmd == "USER")
			code = userCmd(client, args);
		else if (cmd == "PASS")
			code = passCmd(client, args);
		if((client.getRegistryState() && client.getNick() != "" && client.getUser() != "") && !client.getHandShake())
			handshake(client);
		std::cout << RED << "[" << code << "] " << RESET << std::endl;
		return ;
	}
	switch (isCommand(cmd))
	{
		case NICK:
			code = nickCmd(client, args);
			break;
		case USER:
			code = userCmd(client, args);
			break;
		case PASS:
			code = passCmd(client, args);
			break;
		case JOIN:
			code = joinCmd(client, args);
			break;
		case PART:
			// part(client, args);
			break;
		case TOPIC:
			// topic(client, args);
			break;
		case INVITE:
			// invite(client, args);
			break;
		case KICK:
			// kick(client, args);
			break;
		case MODE:
			// mode(client, args);
			break;
		case PRIVMSG:
			code = privmsgCmd(client, args);
			break;
		case OPER:
			code = operCmd(client, args);
			break;
		case HELP:
			code = helpCmd(client, args);
			break;
		case DIE:
			code = dieCmd(client, args);
			break;
		case QUIT:
			code = quitCmd(client, args);
			break;
		default:
		{
			if (client.getRegistryState())
			{
				// ERR_UNKNOWNCOMMAND (421) (send to client)
				code = 421;
				// std::cout << cmd << ": Unknow command\n";
			}
		}
	}
	errorReply(client, code, cmd);
	// ServerErrReply(code, client);
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
    client.setNick(args[0]);
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
	if (args.size() < 4)
		return 461;
		// return reply(client, 461, "", "] USER: Not enough params") ? 0 : -1;
	if (client.getHandShake() == true)
		return 462;
		// return reply(client, 462, args[0], "] USER: already registered") ? 0 : -1;
	client.setUser(args[0]);
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

void	Server::createChannel(std::string channelName)
{
	(void)channelName;
//	Channel *newChannel = new Channel(channelName);


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
	(void)args;
	std::map<std::string, std::string>	*joins;
	
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
		if (!getChannel(joins_it->first)) // si no encuentras el canal
		{
			if (joins_it->first[0] != '#')
			{
				//ERR 403 -> "there is no such channel"
				std::cout << "channel doesnt exist and can't be created\n";
				return (403);
			}
			else
			{
				// create channel
				std::cout << "creating channel...\n";
				createChannel(joins_it->first);
				std::cout << "done creating channel...\n";
				// make client operator
			}
		}
		else // el channel ya exise
		{
			/*if ( client.getChannelsJoined() <= CHANLIMIT - 1)
			{
				// add client to channel
				getChannel(joins_it->first)->addUser(client);
			}
			else
				ERR_TOOMANYCHANNELS (405)
			
			if (client is authorized to join) // cumplen con: key, client limit , ban - exception, invite-only - exception
			{
					
			}
			else
				mirar esos codigos de ERR*/
		}
		joins_it++;
	}	
	delete joins;
	// when all is good and done, SEND various responses to client
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
	std::map<std::string, Channel>::iterator it;
	for (it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
        Channel* chan = get_channel(*it);
        if (chan) {
			std::string quit_line = ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname + " QUIT :" + (args.empty() ? "Leaving" : args[0]);
			// Notify all clients in the channel about the quit
			chan->broadcast(quit_line, client);

			// Remove the client from the channel
            chan->removeUser(&client);
        }
    }
	disconnectClient(client);
	return (0);
}

void Server::disconnectClient(Client &client)
{
	std::cout << "Disconnecting client fd " << client.getFd() << "...\n";

	// Remove the client from all channels they are part of
	std::map<std::string, Channel> channels = client.getChannels();	

    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel* chan = get_channel(*it);
        if (chan) {
            chan->removeUser(&client);
			// Send to client in channel
			reply(client, 404, it->first, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "Disconected ()" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));
			std::cout << "Disconnected ()\n";
            // Optionally broadcast PART or QUIT here
        }
    }
	_clients.erase(client.getFd());
	close(client.getFd());
	std::vector<struct pollfd>::iterator it = _pollFds.begin();
	for(size_t idx = 0; idx < _pollFds.size(); ++idx, ++it)
	{
		if (it->fd == client.getFd())
		{
			_pollFds.erase(it);
			break;
		}
	}
	std::cout << "Client fd " << client.getFd() << " disconnected successfully.\n";
}

Channel *Server::get_channel(const std::pair<std::string, Channel> &pair)
{
	std::map<std::string, Channel *>::iterator it = _channels.find(pair.first);
	if (it != _channels.end())
		return it->second;
	else
	{
		
		return it->second;
	}
}

CommandType Server::isCommand(const std::string &cmd)
{
	if (cmd == "PASS" || cmd == "pass") return (PASS);
	else if (cmd == "NICK" || cmd == "nick") return (NICK);
	else if (cmd == "USER" || cmd == "user") return (USER);
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
	else
		return static_cast<CommandType>(-1); // Unknown command
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
		std::string &tgt = targets[i];
		if (tgt[0] == '#')
		{
			Channel *channel = getChannel(tgt);
			if (!channel)
			{
				errorReply(client, 403, channel->getName());
				// std::cerr << "[" << client.getFd() << "] PRIVMSG: No such channel: " << tgt << "\n";
				continue; // or collect error codes if needed
			}
			channel->broadcast(message, client);
		}
		else
		{
			bool found = false;
			for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				if (it->second->getNick() == tgt)
				{
					sendLine(*it->second, ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname + " PRIVMSG " + tgt + " :" + message + "\r\n");
					found = true;
					break;
				}
			}
			if (!found)
				errorReply(client, 401, tgt);
		}
	}
	// if (target[0] == '#')
	// {
	// 	Channel *channel = getChannel(target);
	// 	if (!channel)
	// 		return 403; // ERR_NOSUCHCHANNEL
	// 	channel->broadcast(message, client);
	// }
	// else
	// {
	// 	std::map<int, Client *>::iterator	it;
	// 	for(it = _clients.begin(); it != _clients.end();)
	// 	{
	// 		if (it->second->getNick() == target)
	// 			break;
	// 		it++;
	// 	}
	// 	if (it != _clients.end())
	// 	{
	// 		sendLine(*it->second, ":" + _hostname + "!" + client.getUser() + "@" + _hostname + " PRIVMSG " + target + " :" + message + "\r\n");
	// 	}
	// 	else
	// 		return 401; // ERR_NOSUCHNICK
	// }
	return 0;
}