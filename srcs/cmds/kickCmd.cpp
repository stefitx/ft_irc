#include "../../inc/Server.hpp"
#include "../../inc/Channel.hpp"

static std::vector<std::string> splitComma(const std::string &s)
{
    std::vector<std::string> out;
    std::stringstream         ss(s);
    std::string               tok;

    while (std::getline(ss, tok, ','))
        if (!tok.empty())
            out.push_back(tok);
    return out;
}

int Server::kickCmd(Client &issuer, std::vector<std::string> args)
{
    if (args.size() < 2)
        return 461;

    const std::string chanName = args[0];
    Channel          *chan     = getChannel(chanName);

    if (!chan)                      return 403;
    if (!chan->isMember(&issuer))   return 442;
    if (!chan->isOperator(&issuer)) return 482;

    std::vector<std::string> victims = splitComma(args[1]);
    if (victims.empty()) return 461;

    std::string reason;
    if (args.size() > 2)
    {
        reason = args[2];
        for (size_t i = 3; i < args.size(); ++i)
            reason += ' ' + args[i];
        if (!reason.empty() && reason[0] == ':')
            reason.erase(0, 1);
    }
    if (reason.size() > KICKLENNUM)
        reason = reason.substr(0, KICKLENNUM);
    for (size_t i = 0; i < victims.size(); ++i)
    {
        const std::string &victimNick = victims[i];
        Client            *victim     = chan->getMembers(victimNick);

        if (!victim)
		{
            std::vector<std::string> a;
            a.push_back(chanName);
            a.push_back(victimNick);
            errorReply(issuer, 441, chanName, a);
            continue;
        }
        std::string kickLine = ":" + issuer.getNick() + '!' + issuer.getUser() + '@' + issuer.getIp() + " KICK " + chanName + ' ' + victimNick + (reason.empty() ? "" : " :" + reason);

        chan->broadcast(kickLine, issuer);
        sendLine(issuer, kickLine + "\r\n");
        chan->removeMember(victim);
        chan->removeOperator(victim);
        victim->removeJoinedChannel(chan);
		pruneChannel(chan);
    }

    return 0;
}
