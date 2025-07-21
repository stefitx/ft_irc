#pragma once

# include <iostream>
# include <vector>
# include <map>
# include "Client.hpp"

class Client;

class	Channel
{
	public:
		Channel(std::string name);
		Channel(const Channel&);
		~Channel();

		Channel	&operator=(const Channel&);

		void	broadcast(std::string msg, Client &client);
		void	addMember(Client *client);
		void	addOperator(Client *client);
		void	removeMember(Client* client);
		void	removeOperator(Client* client);
		std::string	getName() const;

		void	setChanOperator(Client *creator);
		void	setPassword(const std::string key);

		Client*	getMembers(std::string clientName);
		std::map<std::string, Client *>	&getMapMembers();
	private:
		std::string					name;
		std::vector<std::string>	privileges;
		// los privilegios van relacionados con los clientes -> look for a better container
		int							clientNum;
		std::string					topic;
		int							userLimit;
		std::string					password;
		std::string					mode;
		Client*						chanOperator; // the first client to create the channel
		std::string					accessType; // muy relacionado con 'mode'
		
		std::map<std::string, Client *>		members; // map of clients in the channel
		std::map<std::string, Client *>		operators;
};
