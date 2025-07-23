#include "../../inc/Server.hpp"

int Server::passCmd(Client &client, std::vector<std::string> args)
{
	if (client.getRegistryState() == true)
		return 462;
	else if (args.empty())
		return 461;
	else if (args[0] != _password)
		return 464;
	client.setRegistryState(true);
	return (0);

}
