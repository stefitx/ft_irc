/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atudor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 19:46:27 by atudor            #+#    #+#             */
/*   Updated: 2025/07/08 21:22:55 by atudor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "Channel.hpp"
// #include "CommandParser.hpp"

class Client;

class Channel;

class Server
{
	private:
		unsigned short						_port;
		std::string							_password;


		int									_listenFd;
		std::vector<struct pollfd>			_pollFds;

		std::map<int, Client *>				_clients;
		std::map<std::string, Channel *>	_channels;
		bool								_running;
		std::string							_hostname;

		void initListeningSocket();
		void makeSocketNonBlocking(int fd);

 		void acceptNewClient();
		void handleClientData(size_t pollIndex);
		void removeClient(size_t pollIndex);


		// Client *lookupClientByFd(int fd);
		void    processBuffer(Client *c);
		// Channel *getOrCreateChannel(const std::string &name);

		// COMMANDS TO BE RECEIVED
		void	nickCmd(Client&, std::vector<std::string> args);


	public:
		Server(unsigned short port, const std::string &password);
		~Server();
		void run();
} ;
