#include "../inc/Channel.hpp"
# include "../inc/Client.hpp"
# include "../inc/Server.hpp"


Channel::Channel(std::string name) {
	_name = name;
	_clientNum = 0;
	_userLimit = 0;
	_chanOperator = NULL;
	_topic = "";
	_password = "";
	_privileges.clear();
	_mode = "";
	_accessType = "";
	//members = std::map<int, Client *>();
}

Channel::Channel(const Channel &other) :
	_name(other._name),
	_privileges(other._privileges),
	_clientNum(other._clientNum),
	_topic(other._topic),
	_userLimit(other._userLimit),
	_password(other._password),
	_mode(other._mode),
	_chanOperator(other._chanOperator),
	_accessType(other._accessType),
	_members(other._members),
	_operators(other._operators)
{}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name = other._name;
		_clientNum = other._clientNum;
		_chanOperator = other._chanOperator;
		_userLimit = other._userLimit;
		_topic = other._topic;
		_password = other._password;
		_privileges = other._privileges;
		_mode = other._mode;
		_accessType = other._accessType;
		_members = other._members;
		_operators = other._operators;
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
	_clientNum--;
}

void Channel::removeOperator(Client* client)
{
	(void)client;
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

void	Channel::setPassword(const std::string key){
	_password = key;
}

void	Channel::setChanOperator(Client *creator)
{
	_chanOperator = creator;
}

std::map<std::string, Client *>	&Channel::getMapMembers()
{
	return (_members);
}
