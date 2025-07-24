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
        {
            sendLine(client, ":" + _hostname + " " + itoa3(332) + " " + client.getNick() + " " + channel + " " + getChannel(channel)->getTopic() + "\r\n"); // RPL_TOPIC
            sendLine(client, ":" + _hostname + " 333 " + client.getNick() + " " + channel + " " + getChannel(channel)->getTopicSetterMember()->getNick() + "!~" + getChannel(channel)->getTopicSetterMember()->getUser() + "@" + getChannel(channel)->getTopicSetterMember()->getIp() + " " + getChannel(channel)->getTopicSetTime() + "\r\n");
            // >> @time=2025-07-24T20:06:27.016Z :tungsten.libera.chat 333 martalop #holabolamarta martalop!~martalop@195.55.43.195 1753387580
        }
    }
    else
    {
 
        if (getChannel(channel)->getTopicRestrictionMode() && !getChannel(channel)->isChannelOperator(client.getNick()))
            return (errorReply(client, 482, channel, args), 0); // ERR_CHANOPRIVSNEEDED
        
        time_t now = time(NULL);
        std::ostringstream oss;
        oss << now;
        std::string now_str = oss.str();
        getChannel(channel)->setTopic(&args[1][1]);
		getChannel(channel)->setTopicSetterMember(&client);
        getChannel(channel)->setTopicSetTime(now_str);

        std::map<std::string, Client*> members = getChannel(channel)->getMapMembers();
        for (std::map<std::string, Client*>::iterator mem_it = members.begin(); mem_it != members.end(); mem_it++)
        {
            sendLine(*mem_it->second, ":" + mem_it->first + "!~" + mem_it->second->getUser() + "@" + mem_it->second->getIp() + " TOPIC " + channel + " " + getChannel(channel)->getTopic() + "\r\n");
            sendLine(*mem_it->second, ":" + _hostname + " 333 " + mem_it->first + " " + channel + " " + getChannel(channel)->getTopicSetterMember()->getNick() + "!~" + getChannel(channel)->getTopicSetterMember()->getUser() + "@" + getChannel(channel)->getTopicSetterMember()->getIp() + " " + getChannel(channel)->getTopicSetTime() + "\r\n");
        }
    }
    return (0);
}
