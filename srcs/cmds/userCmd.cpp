#include "../../inc/Server.hpp"


int Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (client.getHandShake() == true || !client.getUser().empty())
		return (462);
	if (args.size() < 4)
		return (461);
	std::string user = args[0];
	if (user.empty())
		return (461);
	if (user.size() > USERLENNUM)
		user = user.substr(0, USERLENNUM);
	client.setUser(user);
	if(args[1] == "0")
		client.setHost(inet_ntoa(client.getAddr().sin_addr));
	else
		client.setHost(args[1]);
	return (0);
}
