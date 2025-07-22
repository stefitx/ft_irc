#include "../../inc/Server.hpp"

int Server::operCmd(Client &client, std::vector<std::string> args)
{
	//should we cover ERR_NOOPERHOST (491)? (check the hostname/IP of the client)
	if (args.size() < 2)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] OPER: Not enough params\n";
		return (461);
	}
	std::map<std::string, std::string>::iterator	it;
	for(it = _operator_credentials.begin(); it != _operator_credentials.end();)
	{
		if (it->first == args[0] && it->second == args[1])
			break;
		it++;
	}
	if (it == _operator_credentials.end())
	{
		// ERR_PASSWDMISMATCH (464) -> "<client> :Password incorrect"
		std::cerr << "[" << client.getFd() << "] OPER: wrong credentials...\n";
		return (464);
	}
	client.setNick(args[0]);
	client.setUser(args[0]);
	client.setServerOper(true);
	// RPL_YOUREOPER (381)
	reply(client, 381, "", (client.getIsNetCat() ? std::string(GREEN) : std::string("\00303")) + "You are now an IRC operator" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));
	return (0);
}