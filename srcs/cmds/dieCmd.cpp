#include "../../inc/Server.hpp"

int Server::dieCmd(Client &client)
{
	if (client.getServerOper() == false)
		return (481); // ERR_NOPRIVILEGES (481)
	std::map<int, Client *>::iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		reply(*it->second, 400, "", "Server is shutting down. Goodbye!");
		std::cout << "Notifying client fd " << it->second->getFd() << " about server shutdown...\n";
	}
	stop();
	return (0);
}
