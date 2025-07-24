#include "../../inc/Server.hpp"


int Server::userCmd(Client &client, std::vector<std::string> args)
{
	const int maxLen = 12;

	if (client.getHandShake() == true || !client.getUser().empty())
		return (462);
	if (args.size() < 4)
		return (461);
	std::string user = args[0];
	if (user.empty())
		return (461);
	if (user.size() > maxLen)
		user = user.substr(0, maxLen);
	client.setUser(user);
	if(args[1] == "0")
		client.setHost(inet_ntoa(client.getAddr().sin_addr));
	else
		client.setHost(args[1]);
	return (0);
}
