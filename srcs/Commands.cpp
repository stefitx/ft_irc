#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

int    Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
		return reply(client, 461, "" , "] NICK: not enough params") ? 0 : -1;
	std::map<int, Client*>::iterator	it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == args[0])
			return reply(client, 433, args[0], "] NICK: sorry, nick already in use!") ? 0 : -1;
		it++;
	}
    client.setNick(args[0]);
	return (0);
}

int Server::passCmd(Client &client, std::vector<std::string> args)
{
	if (client.getRegistryState() == true)
		return reply(client, 462, "", "] PASS: stop it, you've already registered") ? 0 : -1;
	else if (args.empty())
		return reply(client, 461, "", "] PASS: empty password!") ? 0 : -1;
	else if (args[0] != _password)
		return reply(client, 464, "", "] PASS: wrong password!") ? 0 : -1;
	client.setRegistryState(true);
	std::cout << "Client fd " << client.getFd() << " authenticated successfully :)\n";
	return (0);

}

int Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 4)
		return reply(client, 461, "", "] USER: Not enough params") ? 0 : -1;
	if (client.getHandShake() == true)
		return reply(client, 462, args[0], "] USER: already registered") ? 0 : -1;
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
