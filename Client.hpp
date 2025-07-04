#pragma once

# include <iostream>
# include "Channel.hpp"

class	Client
{
	public:
		Client();		
		Client(const Client&);
		~Client();

		Client	&operator=(const Client&);
	private:
		std::string			nickName;
		std::string			userName;
		int				ip; // or hostname?
		int				fd;
		int				authState;
		int				registryState;
		std::vector<std::string>	channels;
};
