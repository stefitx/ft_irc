#include "Server.hpp"
#include <iostream>

int	main(int argc, char **argv)
{
	Server	ircServer; 

	// check right number of args
	if (argc != 3)
	{
		std::cout << "Usage: ./irc_server <port> <password>" << std::endl;
		return (1);
	}
	// initialization of signals!
	// initialization of server with argvs (port, pass, etc)
	// creation of socket (socket(), bind(), listen())
	// main listening loop (poll() / epoll() / select())
		// accept() -> returns fds of clients
		// reading and writing to and from clients
		// if authorized, execute commands received
		// else, printerrmsg & continue listening
		// printf in server of the info sent/received
	
	return (0);
}

// when a connection with a client is closed -> close() fd;

// INFO: non-blocking connection means that we always have to be listening or trying to read or write things to all our clients, we shouldn't wait until connection is closed

