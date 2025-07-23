#include "../../inc/Server.hpp"

int Server::operCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return (461); // ERR_NEEDMOREPARAMS (461)

	std::map<std::string, std::string>::iterator	it;
	for(it = _operator_credentials.begin(); it != _operator_credentials.end();)
	{
		if (it->first == args[0] && it->second == args[1])
			break;
		it++;
	}
	if (it == _operator_credentials.end())
		return (464); // ERR_PASSWDMISMATCH (464)
	client.setServerOper(true);
	// RPL_YOUREOPER (381)
	reply(client, 381, "", (client.getIsNetCat() ? std::string(GREEN) : std::string("\00303")) + "You are now an IRC operator" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));
	return (0);
}
