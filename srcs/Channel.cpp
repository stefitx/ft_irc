#include "../inc/Channel.hpp"
# include "../inc/Client.hpp"

Channel::Channel(std::string name) {
	this->name = name;
	clientNum = 0;
	userLimit = 0;
	topic = "";
	password = "";
	privileges.clear();
	mode.clear();
	accessType = "";
	clients = std::map<int, Client *>();
}

Channel::Channel(const Channel &other) :
	name(other.name),
	privileges(other.privileges),
	clientNum(other.clientNum),
	topic(other.topic),
	userLimit(other.userLimit),
	password(other.password),
	mode(other.mode),
	accessType(other.accessType),
	clients(other.clients)
{}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		name = other.name;
		clientNum = other.clientNum;
		userLimit = other.userLimit;
		topic = other.topic;
		password = other.password;
		privileges = other.privileges;
		mode = other.mode;
		accessType = other.accessType;
		clients = other.clients;
	}
	return *this;
}

void	Channel::broadcast(std::string msg, Client &except)
{
	for(std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->first != except.getFd()) // Don't send the message back to the sender
		{
			std::cout << msg << std::endl;
		}
	}
}

void Channel::addUser(Client* client)
{
	(void)client;
}

std::string Channel::getName() const { return name; }

void Channel::removeUser(Client* client)
{
	(void)client;
  //  clients.erase(client->getFd());


}
