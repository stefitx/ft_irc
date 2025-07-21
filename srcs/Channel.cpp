#include "../inc/Channel.hpp"
# include "../inc/Client.hpp"
# include "../inc/Server.hpp"


Channel::Channel(std::string name) {
	this->name = name;
	clientNum = 0;
	userLimit = 0;
	chanOperator = NULL;
	topic = "";
	password = "";
	privileges.clear();
	mode = "";
	accessType = "";
	//members = std::map<int, Client *>();
}

Channel::Channel(const Channel &other) :
	name(other.name),
	privileges(other.privileges),
	clientNum(other.clientNum),
	topic(other.topic),
	userLimit(other.userLimit),
	password(other.password),
	mode(other.mode),
	chanOperator(other.chanOperator),
	accessType(other.accessType),
	members(other.members),
	operators(other.operators)
{}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		name = other.name;
		clientNum = other.clientNum;
		chanOperator = other.chanOperator;
		userLimit = other.userLimit;
		topic = other.topic;
		password = other.password;
		privileges = other.privileges;
		mode = other.mode;
		accessType = other.accessType;
		members = other.members;
		operators = other.operators;
	}
	return *this;
}

void	Channel::broadcast(std::string msg, Client &except)
{
	for(std::map<std::string, Client *>::iterator it = members.begin(); it != members.end(); ++it)
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
	members.insert(std::pair<std::string, Client*>(client->getNick(), client));
	clientNum++;
}

void Channel::addOperator(Client* client)
{
	operators.insert(std::pair<std::string, Client*>(client->getNick(), client));
}

void Channel::removeMember(Client* client)
{
	(void)client;
  //  members.erase(client->getFd());
	members.erase(client->getNick());
	clientNum--;
}

void Channel::removeOperator(Client* client)
{
	(void)client;
	operators.erase(client->getNick());
}

std::string Channel::getName() const { return name; }

Client	*Channel::getMembers(std::string	name)
{
	std::map<std::string, Client *>::iterator	it;

	it = members.find(name);
	if (it == members.end())
		return (NULL);
	return (it->second);
}

void	Channel::setPassword(const std::string key){
	password = key;
}

void	Channel::setChanOperator(Client *creator)
{
	chanOperator = creator;
}

std::map<std::string, Client *>	&Channel::getMapMembers()
{
	return (members);
}
