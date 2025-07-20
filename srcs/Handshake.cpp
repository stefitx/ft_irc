#include "../inc/Server.hpp"

bool Server::sendLine(Client &cli, const std::string &line)
{
	// const char *data;
	// size_t left;
	// ssize_t n;

	// data = line.c_str();
	// left = line.size();
	send(cli.getFd(), line.c_str(), line.size(), 0);
	// while (left)
	// {
	// 	n = send(cli.getFd(), data, left, MSG_DONTWAIT | MSG_NOSIGNAL);
	// 	if (n == -1)
	// 	{
	// 		if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 			continue;
	// 		perror("send");
	// 		return false;
	// 	}
	// 	left  -= n;
	// 	data  += n;
	// }
	return true;
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

	if(difftime(time(NULL), c.getConnectionTime()) > 2)
		c.setIsNetCat(true);
    strftime(datebuf, sizeof datebuf, "%d-%b-%Y %H:%M:%S %Z", localtime(&now));

    reply(c, 1,  "", "Welcome to the " + _hostname + " Network " + c.getNick() + "!~" + c.getUser() + "@" + _hostname);
    reply(c, 2,  "", "Your host is " + _hostname + ", running version 0.1");
    reply(c, 3,  "", "This server was created " + std::string(datebuf));
    reply(c, 4,  _hostname + " 0.1 aiwro imnptkol", "");
    reply(c, 5,  "CHANTYPES=# CHANMODES=,ntkl", "are supported by this server");

    std::cout << GREEN << "[fd " << c.getFd() << "] handshake sent" << RESET << '\n';
}

void Server::errorReply(Client &cli, int code, std::string cmd)
{
	std::string line = ":" + _hostname + " " + itoa3(code) + " " + cli.getNick() + " :" + (cli.getIsNetCat() ? std::string(RED) : std::string("\00304"));
	switch (code)
	{
		case 403: line += "No such channel"; break;
		case 421: line += "[" + cmd + "]: Unknown command"; break;
		case 433: line += "Nickname is already in use"; break;
		case 451: line += "You have not registered"; break;
		case 461: line += "[" + cmd + "]: Not enough parameters"; break;
		case 462: line += "You may not reregister"; break;
		case 464: line += "Password incorrect"; break;
		case 481: line += "Permission Denied- You're not an IRC operator"; break;
		case 524: line = "No help available on this topic"; break;
		default: line += "Unknown error";
	}
	line += (cli.getIsNetCat() ? std::string(RESET) : std::string("\017")) + "\r\n";
	sendLine(cli, line);
}
