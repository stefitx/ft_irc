#include "../../inc/Server.hpp"

static std::string intToStr(int n)
{
	std::ostringstream oss; oss << n; return (oss.str());
}

int Server::modeInitialChecks(Client &client, std::vector<std::string> args, const std::string &chanName, Channel *chan)
{
	if (chanName.empty() || chanName[0] != '#' || !chan)
		return (403);
	if (args.size() == 1)
	{
		std::string line = ":" + _hostname + " 324 " + client.getNick() + " " + chanName + " +" + chan->getMode();
		if (chan->hasMode('k'))
			line += " " + chan->getPassword();
		if (chan->hasMode('l'))
			line += " " + intToStr(chan->getUserLimit());
		sendLine(client, line + "\r\n");
		return (0);
	}
	if (!chan->isOperator(&client))
		return (482);
	std::string flags = args[1];
	if (flags[0] != '+' && flags[0] != '-')
	{
		std::vector<std::string> a; a.push_back(std::string(1, flags[0]));
		return (472);
	}
	return (0);
}


int	Server::modeCmd(Client &client, std::vector<std::string> args)
{	
	char        sign        = '+';
	size_t      paramId     = 2;
	std::string appliedFlags;
	std::vector<std::string> appliedParams;
	bool        needSign    = true;

	if (args.empty())
		return 461;
	const std::string &chanName = args[0];
	Channel *chan = getChannel(chanName);
	int status = modeInitialChecks(client, args, chanName, chan);
	if (status != 0 || args.size() == 1)
		return status;

	std::string flags = args[1];
	for (size_t i = 0; i < flags.size(); ++i)
	{
		char m = flags[i];
		if (m == '+' || m == '-')
		{
			sign     = m;
			needSign = true;
			continue;
		}
		bool didSomething = false;
		switch (m)
		{
			case 'i':
				if (sign == '+')
				{
					chan->addMode('i');
					chan->setInviteMode(true);
				}
				else
				{
					chan->removeMode('i');
					chan->setInviteMode(false);
				}
				didSomething = true;
				break;
			case 't':
				if (sign == '+')
				{
					chan->addMode('t');
					chan->setTopicRestrictionMode(true);
				}
				else
				{
					chan->removeMode('t');
					chan->setTopicRestrictionMode(false);
				}
				didSomething = true;
				break;
			case 'k':
				if (sign == '+')
				{
					if (paramId >= args.size()) return (0);
					chan->setPassword(args[paramId]);
					chan->addMode('k');
					chan->setPasswordMode(true);
					appliedParams.push_back(args[paramId]);
					paramId++;
				}
				else
				{
					chan->setPassword("");
					chan->removeMode('k');
					chan->setPasswordMode(false);
				}
				didSomething = true;
				break;
			case 'l':
				if (sign == '+')
				{
					if (paramId >= args.size()) return (461);
					int lim = std::atoi(args[paramId].c_str());
					if (lim <= 0) return (461);
					chan->setUserLimit(lim);
					chan->addMode('l');
					chan->setUserLimitMode(true);
					appliedParams.push_back(args[paramId]);
					paramId++;
				}
				else
				{
					if (chan->getUserLimit() == 0)
						return (0);
					chan->setUserLimit(0);
					chan->removeMode('l');
					chan->setUserLimitMode(false);
				}
				didSomething = true;
				break;
			case 'o':
			{
				if (paramId >= args.size()) return (461);
				std::string victimNick = args[paramId++];
				Client *victim = chan->getMembers(victimNick);
				if (!victim)
				{
					std::vector<std::string> a; a.push_back(victimNick);
					errorReply(client, 441, chanName, a);   // ERR_USERNOTINCHANNEL
					break;
				}
				(sign == '+') ? chan->addOperator(victim) : chan->removeOperator(victim);
				appliedParams.push_back(victimNick);
				didSomething = true;
				break;
			}
			default:
			{
				std::vector<std::string> a; a.push_back(std::string(1, m));
				errorReply(client, 472, chanName, a);
				break;
			}
		}
		if (didSomething)
		{
			if (needSign)
			{
				appliedFlags += sign;
				needSign = false;
			}
			appliedFlags += m;
		}
	}
	if (appliedFlags.empty())
		return (0);
	std::string modeLine = ":" + client.getNick() + "!~" + client.getUser()
	                     + "@" + client.getIp() + " MODE " + chanName + " "
	                     + appliedFlags;

	for (size_t i = 0; i < appliedParams.size(); ++i)
		modeLine += " " + appliedParams[i];
	chan->broadcast(modeLine, client);
	sendLine(client, modeLine + "\r\n");
	return (0);
}
