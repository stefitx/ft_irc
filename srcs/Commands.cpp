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
		case KICK: code = kickCmd(client, args); break;
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

Channel	*Server::getChannel(const std::string	&name)
{
	std::map<std::string, Channel *>::iterator	it;

	it = _channels.find(name);
	if (it == _channels.end())
		return (NULL);
	return (it->second);
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
