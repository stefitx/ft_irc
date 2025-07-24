#include "../../inc/Server.hpp"

int Server::topicCmd(Client &client, std::vector<std::string> args)
{
    if (args.empty())
        return (461); // ERR_NEEDMOREPARAMS 

    std::string::size_type found = args[0].find(','); 
    if (found != std::string::npos)
        args[0].erase((args[0].begin() + found), args[0].end()); // erase everything after ','

    if (!getChannel(args[0]))
        return (403); // ERR_NOSUCHCHANNEL
    if (!client.isChannelMember(args[0]))
        return (442); // ERR_NOTONCHANNEL
        
    if (args.size() < 2) // there is no 'topic' arg
    {

        if (getChannel(args[0])->getTopic().size() == 0) // si _topic estaba vacio
            sendLine(client, ":" + _hostname + " " + itoa3(331) + " " + client.getNick() + " " + args[0] + " :No topic is set\r\n"); // RPL_NOTOPIC 
        else 
            sendLine(client, ":" + _hostname + " " + itoa3(332) + " " + client.getNick() + " " + args[0] + " " + getChannel(args[0])->getTopic() + "\r\n"); // RPL_TOPIC
    }
    else
    {
        if (getChannel(args[0])->getTopicRestrictionMode() && !getChannel(args[0])->isChannelOperator(client.getNick()))
            return (errorReply(client, 482, args[0], args), 0); // ERR_CHANOPRIVSNEEDED
        getChannel(args[0])->setTopic(&args[1][1]);
        sendLine(client, ":" + client.getNick() + "!" + client.getUser() + "@" + client.getIp() + " TOPIC " + args[0] + " " + getChannel(args[0])->getTopic() + "\r\n");
    }
    return (0);
}