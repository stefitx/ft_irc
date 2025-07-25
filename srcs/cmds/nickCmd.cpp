#include "../../inc/Server.hpp"

int Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.empty())
		return 461; // ERR_NEEDMOREPARAMS

	std::string &newNick = args[0];

	if (newNick.size() > NICKLENNBR)
		newNick = newNick.substr(0, NICKLENNBR);
	std::string oldNick = client.getNick();
	if (oldNick == newNick)
		return 0;
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		if (it->second->getNick() == newNick)
			return 433; // ERR_NICKNAMEINUSE
	client.setNick(newNick);
	std::string nickMsg = ":" + oldNick + "!~" + client.getUser() + "@" +
						  client.getIp() + " NICK :" + newNick;
	if(!client.getHandShake())
		return 0;
	else
		sendLine(client, nickMsg + "\r\n");

	std::map<std::string, Channel*>& chans = client.getChannels();
	for (std::map<std::string, Channel*>::iterator chanPair = chans.begin(); chanPair != chans.end(); ++chanPair)
	{
		Channel *chan = chanPair->second;

		chan->getMapMembers().erase(oldNick);
		chan->getMapMembers()[newNick] = &client;

		std::map<std::string, Client *> &ops = chan->getMapOperators();
		if (ops.erase(oldNick))
			ops[newNick] = &client;
		chan->broadcast(nickMsg,client);
	}
	return 0;
}
