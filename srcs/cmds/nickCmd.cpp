#include "../../inc/Server.hpp"

int Server::nickCmd(Client &client, std::vector<std::string> args)
{
    if (args.empty())
        return 461; // ERR_NEEDMOREPARAMS

    const std::string &newNick = args[0];

    // 1. refuse if the nick is already taken
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        if (it->second->getNick() == newNick)
            return 433; // ERR_NICKNAMEINUSE

    std::string oldNick = client.getNick();
    if (oldNick == newNick)
        return 0;

    client.setNick(newNick);

    // 2. build the NICK message once so we can reuse it
    std::string nickMsg = ":" + oldNick + "!" + client.getUser() + "@" +
                          client.getIp() + " NICK :" + newNick;

    // 3. tell the user themselves
    sendLine(client, nickMsg + "\r\n");

    // 4. update every channel’s bookkeeping and broadcast to everyone there
    std::map<std::string, Channel*>& chans = client.getChannels();
    for (std::map<std::string, Channel*>::iterator chanPair = chans.begin(); chanPair != chans.end(); ++chanPair)
    {
        Channel *chan = chanPair->second;

        // rename inside members/ops maps
        chan->getMapMembers().erase(oldNick);
        chan->getMapMembers()[newNick] = &client;

        std::map<std::string, Client *> &ops = chan->getMapOperators();
        if (ops.erase(oldNick))
            ops[newNick] = &client;

        // send to all members, *including* the origin
        chan->broadcast(nickMsg,client);
    }

    return 0;
}