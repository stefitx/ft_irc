#include "../../inc/Server.hpp"

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