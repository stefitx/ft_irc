#pragma once

# include <iostream>
# include <vector>
# include <map>
# include "Client.hpp"

class	Channel
{
	public:
		Channel(std::string name);
		Channel(const Channel&);
		~Channel();

		Channel	&operator=(const Channel&);
	private:
		std::string			name;
		// std::map<int, Client>	clients; // int fd of the client
		std::vector<std::string>	privileges;
		// los privilegios van relacionados con los clientes -> look for a better container
		int				clientNum;
		std::string			topic;
		int				userLimit;
		std::string			password;
		std::vector<int>		mode;
		// again, not sure which container to use
		std::string			accessType; // muy relacionado con 'mode'
};
