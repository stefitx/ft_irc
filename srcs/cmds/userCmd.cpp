#include "../../inc/Server.hpp"


int Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (client.getHandShake() == true)
		return 462;

	if (args.size() < 4)
		return 461;
	client.setUser(args[0]);
	client.setHost(inet_ntoa(client.getAddr().sin_addr));
	std::cout << "Client fd " << client.getFd() << " ip: " << client.getIp() << std::endl;
	return (0);
}