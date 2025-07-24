#include "../../inc/Server.hpp"

int Server::dieCmd(Client &client)
{
	if (client.getServerOper() == false)
		return (481); // ERR_NOPRIVILEGES (481)
	std::map<int, Client *>::iterator it;
	for(it = _clients.begin(); it != _clients.end(); ++it)
	{
		noticeCmd(*it->second, vectorSplit(it->second->getNick() + " " + colorLine(*it->second, "Notifying", "RED") + " " + colorLine(*it->second, it->second->getNick(), "BLUE") + " " + colorLine(*it->second, "about server shutdown... Goodbye!", "RED"), ' '));
		std::cout << "Notifying client fd " << it->second->getFd() << " about server shutdown...\n";
	}
	stop();
	return (0);
}
