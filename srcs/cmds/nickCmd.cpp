#include "../../inc/Server.hpp"

int    Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
		return 461;
	std::map<int, Client*>::iterator	it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == args[0])
			return 433;
		it++;
	}
	std::string oldNick = client.getNick();
	std::cout << "Nick changed from " << oldNick << " to " << args[0] << "\n";
	client.setNick(args[0]);
	   for (std::map<std::string, Channel *>::iterator itChan = client.getChannels().begin(); itChan != client.getChannels().end(); ++itChan)
	   {
			   std::map<std::string, Client *> members = itChan->second->getMapMembers();
			   members.erase(oldNick);
			   std::map<std::string, Client *> operators = itChan->second->getMapOperators();
			   std::map<std::string, Client *>::iterator opIt = operators.find(oldNick);
			   if (opIt != operators.end()) {
					   opIt->second->setNick(args[0]);
					   operators.erase(opIt);
			   }
			   itChan->second->broadcast(":" + oldNick + " NICK :" + client.getNick(), client);
	   }
	std::cout << "Registry state: " << client.getRegistryState() << ", Handshake: " << client.getHandShake() << "\n";
	return (0);
}