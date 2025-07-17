#include "../inc/Server.hpp"

void	Server::executeCmd(Client &client, std::string cmd, std::vector<std::string> args)
{
	int code = 0;
	args.erase(args.begin());
	if (!client.getHandShake() && cmd != "PASS" && cmd != "NICK" && cmd != "USER")
	{
		reply(client, 451, "", "You have not registered");
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

static std::string itoa3(int code)
{
	std::ostringstream oss;

	if (code < 10)
		oss << "00";
	else if (code < 100)
		oss << '0';
	oss << code;
	return oss.str();
}

bool Server::reply(Client &cli, int code, const std::string &params, const std::string &text)
{
	std::string line = ":" + _hostname + " " + itoa3(code) + " " + cli.getNick() + " ";
    if (!params.empty())
        line += " " + params;
    if (!text.empty())
        line += ":" + text;
    line += "\r\n"; 
	return (sendLine(cli, line));
}

void Server::handshake(Client &c)
{
    c.setHandShake(true);

    time_t now = time(NULL);
    char   datebuf[64];

    strftime(datebuf, sizeof datebuf, "%d-%b-%Y %H:%M:%S %Z", localtime(&now));

    reply(c, 1,  "", "Welcome to the " + _hostname + " Network " + c.getNick() + "!~" + c.getUser() + "@" + _hostname);
    reply(c, 2,  "", "Your host is " + _hostname + ", running version 0.1");
    reply(c, 3,  "", "This server was created " + std::string(datebuf));
    reply(c, 4,  _hostname + " 0.1 aiwro imnptkol", "");
    reply(c, 5,  "CHANTYPES=# CHANMODES=,ntkl", "are supported by this server");

    std::cout << GREEN << "[fd " << c.getFd() << "] handshake sent" << RESET << '\n';
}

// void Server::handshake(Client &client)
// {
// 	// SEND TO CLIENT THE HANDSHAKE MESSAGE
// 	client.setHandShake(true);
// 	std::cout << GREEN << "RULE 001 : <client> :Welcome to the <networkname> Network, <nick>[!<user>@<host>]" << RESET << std::endl;
// 	std::cout << GREEN << "RULE 002 : <client> :Your host is <servername>, running version <version>" << RESET << std::endl;
// 	std::cout << GREEN << "RULE 003 : <client> :This server was created <datetime>" << RESET << std::endl;
// 	std::cout << GREEN << "RULE 004 : <client> <servername> <version> <available user modes> \n <available channel modes> [<channel modes with a parameter>]" << RESET << std::endl;
// 	std::cout << GREEN << "RULE 005 : <client> <1-13 tokens> :are supported by this server" << RESET << std::endl;
// }
