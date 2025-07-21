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
		void	addUser(Client *client);
		void	removeUser(Client* client);
		std::string	getName() const;


	private:
		std::string					name;
		std::vector<std::string>	privileges;
		// los privilegios van relacionados con los clientes -> look for a better container
		int							clientNum;
		std::string					topic;
		int							userLimit;
		std::string					password;
		std::vector<int>			mode;
		// again, not sure which container to use
		std::string					accessType; // muy relacionado con 'mode'
		
		// !!
		std::map<int, Client *>		clients; // map of clients in the channel
		// maybe haria este map <string, Client *>!

};
