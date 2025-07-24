#include "../inc/Server.hpp"

std::string Server::colorLine(Client &client, const std::string &line, const std::string &color)
{
	if (color == "WHITE")
		return ((client.getIsNetCat() ? std::string(WHITE) : std::string("\00300"))) + line  + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317"));
	else if (color == "BLACK")
		return ((client.getIsNetCat() ? std::string(BLACK) : std::string("\00301")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "BLUE")
		return ((client.getIsNetCat() ? std::string(BLUE) : std::string("\00302")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "GREEN")
		return ((client.getIsNetCat() ? std::string(GREEN) : std::string("\00303")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if(color == "RED")
		return ((client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "PURPLE" || color == "MAGENTA")
		return ((client.getIsNetCat() ? std::string(MAGENTA) : std::string("\00306")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "ORANGE")
		return ((client.getIsNetCat() ? std::string(ORANGE) : std::string("\00307")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "YELLOW")
		return ((client.getIsNetCat() ? std::string(YELLOW) : std::string("\00308")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if (color == "CYAN")
		return ((client.getIsNetCat() ? std::string(CYAN) : std::string("\00311")) + line + (client.getIsNetCat() ? std::string(RESET) : std::string("\00317")));
	else if(color == "RESET")
		return "\017" + line;
	else
		return line; // No color applied
}

bool sendLine(Client &cli, const std::string &line)
{
	const char *data;
	size_t left;
	ssize_t n;

	data = line.c_str();
	left = line.size();
	// n = send(cli.getFd(), line.c_str(), line.size(), 0);
	// if (n == -1)
	// {
	// 	if (errno == EAGAIN || errno == EWOULDBLOCK)
	// 		return false;
	// 	perror("send");
	// 	return false;
	// }
	while (left)
	{
		n = send(cli.getFd(), data, left, MSG_DONTWAIT | MSG_NOSIGNAL);
		if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue;
			perror("send");
			return false;
		}
		left  -= n;
		data  += n;
	}
	return true;
}

std::string Server::itoa3(int code)
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
    reply(c, 5,  "USERLEN=12 CHANTYPES=# CHANMODES=,ntkl", "are supported by this server");
	noticeCmd(c, vectorSplit(c.getNick() + " " + colorLine(c, "Hello", "GREEN") + " " + colorLine(c, c.getNick(), "BLUE") + colorLine(c, ", Welcome to our IRC server! Hope you enjoy.", "GREEN"), ' '));
}

void Server::errorReply(Client &cli, int code, std::string str, std::vector<std::string> args)
{
	std::string line = ":" + _hostname + " " + itoa3(code) + " " + (cli.getNick().empty() ? "*" : cli.getNick());
	switch (code)
	{
		case 000: return; // No error
		case 401: line += " :" + str + ": No such nick/channel"; break;
		case 403: line += " :" + args[0] + ": No such channel"; break;
		case 405: line += " " + args[0] + " :You have joined too many channels"; break;
		case 411: line += " :No recipient given (" + str + ")"; break;
		case 412: line += " :No text to send"; break;
		case 421: line += " :[" + str + "]: Unknown command"; break;
		case 433: line += " :" + args[0] + ": Nickname is already in use"; break;
		case 442: line += args[0] + ": You're not on that channel"; break;
		case 451: line += " :You have not registered"; break;
		case 461: line += " :[" + str + "]: Not enough parameters"; break;
		case 462: line += " :You may not reregister"; break;
		case 464: line += " :Password incorrect"; break;
		case 471: line += str + " :Cannot join channel (+l) - channel is full, try again later"; break;
		case 473: line += str + " :Cannot join channel (+i)"; break;
		case 475: line += str + " :Cannot join channel (+k)"; break;
		case 481: line += " :Permission Denied- You're not an IRC operator"; break;
		case 482: line += " " + args[0] + " :You're not channel operator"; break;
		case 524: line = " :" + args[0] + ": No help available on this topic"; break;
		case 441: line += args[1] + " " + args[0] + " :They aren't on that channel"; break;
		case 472: line += args[0] + " :is an unknown mode"; break;
		case 502: line += " :Cannot change mode for that channel"; break;
		default: line += " :Unknown error";
	}
	line +=  "\r\n";
	sendLine(cli, line);
}
