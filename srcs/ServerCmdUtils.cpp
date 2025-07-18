#include "../inc/Server.hpp"

void	Server::executeCmd(Client &client, std::string cmd, std::vector<std::string> args)
{
	int code = 0;
	args.erase(args.begin());
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
	if (cmd == "JOIN")
	{
		joinCmd(client, args);
		return ;
	}

	switch (isComand(cmd))
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
			// join(client, args);
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
			// privmsg(client, args);
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
	}
	// podriamos ponerle un codigo de retorno a los cmds y liego llamar a:
	// ServerReply(code, client);
}

void Server::handshake(Client &client)
{
	// SEND TO CLIENT THE HANDSHAKE MESSAGE
	client.setHandShake(true);
	std::cout << GREEN << "RULE 001 : <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]" << RESET << std::endl;
	std::cout << GREEN << "RULE 002 : <client> :Your host is <servername>, running version <version>" << RESET << std::endl;
	std::cout << GREEN << "RULE 003 : <client> :This server was created <datetime>" << RESET << std::endl;
	std::cout << GREEN << "RULE 004 : <client> <servername> <version> <available user modes> \n <available channel modes> [<channel modes with a parameter>]" << RESET << std::endl;
	std::cout << GREEN << "RULE 005 : <client> <1-13 tokens> :are supported by this server" << RESET << std::endl;
}

int    Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() == 0)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] NICK: not enough params" << std::endl;
		return (461);
	}
	std::map<int, Client*>::iterator	it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == args[0])
		{
			// ERR_NICKNAMEINUSE (433) -> "<client> <nick> :Nickname is already in use"
			std::cerr << "[" << client.getFd() << "] NICK: sorry, nick already in use!\n";
			return (433);
		}
		it++;
	}
    client.setNick(args[0]);
	return (0);
}

int Server::passCmd(Client &client, std::vector<std::string> args)
{
	if (client.getRegistryState() == true)
	{
		// ERR_ALREADYREGISTERED (462) -> "<client> :You may not reregister"
		std::cerr << "[" << client.getFd() << "] PASS: stop it, you've already registered\n";
		return (462);
	}
	else if (!args.size())
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] PASS: empty password!\n";
		return (461);
	}
	else if (args[0] != _password)
	{
		// ERR_PASSWDMISMATCH (464) -> "<client> :Password incorrect"
		std::cerr << "[" << client.getFd() << "] PASS: wrong password...\n";
		return (464);
	}
	client.setRegistryState(true);
	std::cout << "Client fd " << client.getFd() << " authenticated successfully :)\n";
	return (0);

}

int Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 4)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] USER: Not enough params\n";
		return (461);
	}
	if (client.getHandShake() == true)
	{
		// ERR_ALREADYREGISTERED (462) -> "<client> :You may not reregister"
		std::cerr << "[" << client.getFd() << "] USER: already registered\n";
		return (462);
	}
	client.setUser(args[0]);
	return (0);
}

int	Server::joinCmd(Client &client, std::vector<std::string> args)
{
	(void)client;
	(void)args;
	/*if (server._channels.getName() == args[0])
	{
			channel already exists
	}
	else
	{	create channel
			or
		ERR 403 -> "there is no such channel"
	}
	if (client is authorized to join) // cumplen con: key, client limit , ban - exception, invite-only - exception
	{
		if ( el num de channels que se ha unido el cliente es < CHANLIMIT RPL_ISUPPORT)
		{
			add client to channel
		}
		else
		 	ERR_TOOMANYCHANNELS (405)
	}
	else
		mirar esos codigos de ERR*/
		
	// when all is good and done, SEND response to client -> >> @time=2025-07-17T14:09:44.581Z :martalop!~martalop@195.55.43.195 JOIN #hi * :realname
	return (0);
}

int Server::helpCmd(Client &client, std::vector<std::string> args)
{
	(void)client;
	//we have to send all the prints to the client
	if(args.empty())
	{
		std::cout << "Commands Available:\n";
		std::cout << "NICK \t\t USER \t\t PASS \t\t QUIT \n";
		std::cout << "JOIN \t\t PART \t\t TOPIC \t\t INVITE \n";
		std::cout << "KICK \t\t MODE \t\t PRIVMSG \t\t OPER\n";
		std::cout << "Type /WELP <command> for more information, or /WELP -l\n";
	}
	else if (args[0] == "-l")
	{
		std::cout << "Commands Available:\n";
		std::cout << RED << "NICK" << RESET << " : Usage: NICK <nickname>, sets your nick\n";
		std::cout << RED << "USER" << RESET << " : Usage: USER <username> <hostname> <servername> :<realname>\n";
		std::cout << RED << "PASS"<< RESET << " : Usage: PASS <password>\n";
		std::cout << RED << "QUIT" << RESET << " : Usage: QUIT [<reason>], disconnects from the current server\n";
		std::cout << RED << "JOIN" << RESET << " : Usage: JOIN <channel>, joins the channel\n";
		std::cout << RED << "PART" << RESET << " : Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\n";
		std::cout << RED << "TOPIC" << RESET << " : Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\n";
		std::cout << RED << "INVITE" << RESET << " : Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\n";
		std::cout << RED << "KICK" << RESET << " : Usage: KICK <nick> [reason], kicks the nick from the current channel (needs chanop)\n";
		std::cout << RED << "MODE" << RESET << " : Usage: MODE <channel> [<mode>]\n";
		std::cout << RED << "PRIVMSG" << RESET << " : Usage: PRIVMSG <target> :<message>\n";
		std::cout << RED << "OPER" << RESET << " : Usage: OPER <username> <password>, grants operator privileges\n";
		std::cout << "Type /WELP <command> for more information\n";
	}
	else
	{
		std::cout << "Command: " << args[0] << "\n";
		if (args[0] == "NICK" || args[0] == "nick")
			std::cout << "Usage: NICK <nickname>, sets your nick\n";
		else if (args[0] == "USER" || args[0] == "user")
			std::cout << "Usage: USER <username> <hostname> <servername> :<realname>\n";
		else if (args[0] == "PASS" || args[0] == "pass")
			std::cout << "Usage: PASS <password>\n";
		else if (args[0] == "QUIT" || args[0] == "quit")
			std::cout << "Usage: QUIT [<reason>], disconnects from the current server\n";
		else if (args[0] == "JOIN" || args[0] == "join")
			std::cout << "Usage: JOIN <channel>, joins the channel\n";
		else if (args[0] == "PART" || args[0] == "part")
			std::cout << "Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\n";
		else if (args[0] == "TOPIC" || args[0] == "topic")
			std::cout << "Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\n";
		else if (args[0] == "INVITE" || args[0] == "invite")
			std::cout << "Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\n";
		else if (args[0] == "KICK" || args[0] == "kick")
			std::cout << "Usage: KICK <nick> [reason], kicks the nick from the current channel (needs chanop)\n";
		else if (args[0] == "MODE" || args[0] == "mode")
			std::cout << "Usage: MODE <channel> [<mode>]\n";
		else if (args[0] == "PRIVMSG" || args[0] == "privmsg")
			std::cout << "Usage: PRIVMSG <target> :<message>\n";
		else if (args[0] == "OPER" || args[0] == "oper")
			std::cout << "Usage: OPER <username> <password>, grants operator privileges\n";
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
	std::cout << "Client fd " << client.getFd() << " is now an operator :)\n";
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
		// Notify each client about the server shutdown with a SEND
		std::cout << "Notifying client fd " << it->second->getFd() << " about server shutdown...\n";
	}
	exit(0);
	return (0);
}

int Server::quitCmd(Client &client, std::vector<std::string> args)
{
	for (std::map<std::string, Channel>::iterator it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
        Channel* chan = get_channel(*it);
        if (chan) {
			std::string quit_line = ":" + client.getNick() + "!" + client.getUser() + "@" + _hostname + " QUIT :" + (args.empty() ? "Leaving" : args[0]);
			// Notify all clients in the channel about the quit
			chan->broadcast(quit_line, client);

			// Remove the client from the channel
            chan->remove_user(&client);
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
            chan->remove_user(&client);
			// Send to client in channel
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



CommandType Server::isComand(const std::string &cmd)
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
	else
		return static_cast<CommandType>(-1); // Unknown command
}
