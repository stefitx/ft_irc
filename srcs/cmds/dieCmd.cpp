#include "../../inc/Server.hpp"

int Server::dieCmd(Client &client, std::vector<std::string> args)
{
	(void)args;
	if (client.getServerOper() == false)
	{
		// ERR_NOPRIVILEGES (481) -> "<client> :Permission Denied- You're not an IRC operator"
		std::cerr << "[" << client.getFd() << "] DIE: You are not an operator!\n";
		return (481);
	}
	std::map<int, Client *>::iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		reply(*it->second, 400, "", "Server is shutting down. Goodbye!");
		std::cout << "Notifying client fd " << it->second->getFd() << " about server shutdown...\n";
	}
	stop();
	return (0);
}
