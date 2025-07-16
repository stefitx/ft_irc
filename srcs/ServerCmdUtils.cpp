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
	// switch (isComand(cmd))
	// {
	// 	case QUIT:
	// 		// quit(client, args);
	// 		break;
	// 	case JOIN:
	// 		// join(client, args);
	// 		break;
	// 	case PART:
	// 		// part(client, args);
	// 		break;
	// 	case TOPIC:
	// 		// topic(client, args);
	// 		break;
	// 	case INVITE:
	// 		// invite(client, args);
	// 		break;
	// 	case KICK:
	// 		// kick(client, args);
	// 		break;
	// 	case MODE:
	// 		// mode(client, args);
	// 		break;
	// 	case PRIVMSG:
	// 		// privmsg(client, args);
	// 		break;
	// }
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

CommandType Server::isComand(const std::string &cmd)
{
	if (cmd == "PASS") return (PASS);
	else if (cmd == "NICK") return (NICK);
	else if (cmd == "USER") return (USER);
	else if (cmd == "QUIT") return (QUIT);
	else if (cmd == "JOIN") return (JOIN);
	else if (cmd == "PART") return (PART);
	else if (cmd == "TOPIC") return (TOPIC);
	else if (cmd == "INVITE") return (INVITE);
	else if (cmd == "KICK") return (KICK);
	else if (cmd == "MODE") return (MODE);
	else if (cmd == "PRIVMSG" || cmd == "privmsg")return (PRIVMSG);
	else
		return static_cast<CommandType>(-1); // Unknown command
}
