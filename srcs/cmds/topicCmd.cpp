#include "../../inc/Server.hpp"

int Server::topicCmd(Client &client, std::vector<std::string> args)
{
    if (args.empty())
        return (461); // ERR_NEEDMOREPARAMS

    std::string channel = args[0];
    std::string::size_type found = channel.find(',');
    if (found != std::string::npos)
        channel.erase((channel.begin() + found), channel.end()); // erase everything after ','

    if (!getChannel(channel))
        return (errorReply(client, 403, "", vectorSplit(args[0], ',')), 0); // (403) ERR_NOSUCHCHANNEL
       
    if (!client.isChannelMember(channel))
        return (errorReply(client, 442, "", vectorSplit(args[0], ',')), 0); // (442) ERR_NOTONCHANNEL

    if (args.size() < 2) // there is no 'topic' arg
    {

        if (getChannel(channel)->getTopic().size() == 0) // si _topic estaba vacio
            sendLine(client, ":" + _hostname + " " + itoa3(331) + " " + client.getNick() + " " + channel + " :No topic is set\r\n"); // RPL_NOTOPIC
        else
            sendLine(client, ":" + _hostname + " " + itoa3(332) + " " + client.getNick() + " " + channel + " " + getChannel(channel)->getTopic() + "\r\n"); // RPL_TOPIC
    }
    else
    {
        if (getChannel(channel)->getTopicRestrictionMode() && !getChannel(channel)->isChannelOperator(client.getNick()))
            return (errorReply(client, 482, channel, args), 0); // ERR_CHANOPRIVSNEEDED
        getChannel(channel)->setTopic(&args[1][1]);
        sendLine(client, ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " TOPIC " + channel + " " + getChannel(channel)->getTopic() + "\r\n");
    }
    return (0);
}
