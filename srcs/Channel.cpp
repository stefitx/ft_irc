#include "../inc/Channel.hpp"
# include "../inc/Client.hpp"
# include "../inc/Server.hpp"


Channel::Channel(std::string name) {
	_name = name;
	_clientNum = 0;
	_password = "";
	_userLimit = 0;
	_topic = "";
	_mode = "";
	_invitedUsers.clear();
	_passwordMode = false;
	_inviteMode = false;
	_topicRestrictionMode = true;
	_userLimitMode = false;
}

Channel::Channel(const Channel &other) :
	_name(other._name),
	_password(other._password),
	_clientNum(other._clientNum),
	_topic(other._topic),
	_userLimit(other._userLimit),
	_invitedUsers(other._invitedUsers),
	_mode(other._mode),
	_members(other._members),
	_operators(other._operators)
{}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name = other._name;
		_clientNum = other._clientNum;
		_userLimit = other._userLimit;
		_topic = other._topic;
		_password = other._password;
		_mode = other._mode;
		_members = other._members;
		_operators = other._operators;
		_invitedUsers = other._invitedUsers;
	}
	return *this;
}

void	Channel::broadcast(std::string msg, Client &except)
{
	for(std::map<std::string, Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (it->first != except.getNick()) // Don't send the message back to the sender
		{
			sendLine(*(it->second), msg + "\r\n");
			std::cout << msg << std::endl;
		}
	}
}

void Channel::addMember(Client* client)
{
	_members.insert(std::pair<std::string, Client*>(client->getNick(), client));
	_clientNum++;
}

void Channel::addOperator(Client* client)
{
	_operators.insert(std::pair<std::string, Client*>(client->getNick(), client));
}

void Channel::removeMember(Client* client)
{
	_members.erase(client->getNick());
	for(std::map<std::string, Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (it->second == client)
		{
			removeOperator(client);
			break;
		}
	}
	_clientNum--;
}

void Channel::removeOperator(Client* client)
{
	_operators.erase(client->getNick());
}

std::string Channel::getName() const { return _name; }

Client	*Channel::getMembers(std::string	name)
{
	std::map<std::string, Client *>::iterator	it;

	it = _members.find(name);
	if (it == _members.end())
		return (NULL);
	return (it->second);
}

Client* Channel::getOperators(std::string name)
{
	std::map<std::string, Client *>::iterator	it;

	it = _operators.find(name);
	if (it == _operators.end())
		return (NULL);
	return (it->second);
}

bool	Channel::isChannelOperator(std::string memberName)
{
	if (getOperators(memberName))
		return (true);
	return (false);
}

bool	Channel::isInvitedUser(Client *user)
{
	for (std::vector<Client *>::iterator it = _invitedUsers.begin(); it != _invitedUsers.end(); it++)
	{
		if (*it == user)
			return (true);
	}
	return (false);
}

std::map<std::string, Client *> &Channel::getMapOperators()
{
	return _operators;
}

void	Channel::setPassword(const std::string key){
	_password = key;
}

void	Channel::setMode(const std::string mode)
{
	_mode = mode;
	for (size_t i = 0; i < mode.size(); i++)
	{
		if (mode[i] == 'i')
			_inviteMode = true;
		if (mode[i] == 't')
			_topicRestrictionMode = true;
		if (mode[i] == 'k')
			_passwordMode = true;
		if (mode[i] == 'l')
			_userLimitMode = true;
	}
}

std::map<std::string, Client *>	&Channel::getMapMembers()
{
	return (_members);
}

std::string	Channel::getTopic(){
	return _topic;
}

int	Channel::authorizedToJoin(Client *client, std::string key)
{
	if (_passwordMode) // (+k)
	{
		if (key.empty() || key != _password)
			return (475); // ERR_BADCHANNELKEY
	}
	if (_userLimitMode && _clientNum + 1 >= _userLimit) // (+l)
		return (471); // ERR_CHANNELISFULL
	if (_inviteMode && !isInvitedUser(client))  //(+i)
		return (473); // ERR_INVITEONLYCHAN
	return (0);
}

bool Channel::isMember(Client *c) const
{
    return _members.find(c->getNick()) != _members.end();
}

bool Channel::isOperator(Client *c) const
{
    if (_operators.find(c->getNick()) != _operators.end())
		return true;
	return false;
}

bool Channel::isInviteMode()
{
	return _inviteMode;
}

void Channel::addIvitedUser(Client *client)
{
	_invitedUsers.push_back(client);
}
