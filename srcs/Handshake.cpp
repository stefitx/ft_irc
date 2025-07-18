#include "../inc/Server.hpp"

bool Server::sendLine(Client &cli, const std::string &line)
{
	const char *data;
	size_t left;
	ssize_t n;

	data = line.c_str();
	left = line.size();
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
