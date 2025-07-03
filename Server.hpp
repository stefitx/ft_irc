#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>

class	Server
{
	public:
		Server();		
		Server(const Server&);
		~Server();

		Server	&operator=(const Server&);
	private:
		int	portNumber;
		int	socketFd;
		
		//array of Clients()?
		//array of Channels()?
};

#endif
