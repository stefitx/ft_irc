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
#include "Colors.h"
// #include "CommandParser.hpp"

enum CommandType
{
	PASS,
	NICK,
	USER,
	HELP,
	QUIT,
	JOIN,
	PART,
	TOPIC,
	INVITE,
	KICK,
	MODE,
	PRIVMSG,
	OPER,
	DIE
	// NOTICE
};


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
		std::map<std::string, std::string>	_operator_credentials;

		void initListeningSocket();
		void makeSocketNonBlocking(int fd);

 		void acceptNewClient();
		void handleClientData(size_t pollIndex);
		void removeClient(size_t pollIndex);


		// Client *lookupClientByFd(int fd);
		void    processBuffer(Client *c);
		// Channel *getOrCreateChannel(const std::string &name);

		void	executeCmd(Client &, std::string cmd, std::vector<std::string> args);
		void	handshake(Client &client);
		CommandType isComand(const std::string &cmd);

		// COMMANDS TO BE RECEIVED
		int	nickCmd(Client&, std::vector<std::string> args);
		int	userCmd(Client&, std::vector<std::string> args);
		int	passCmd(Client&, std::vector<std::string> args);
		int	helpCmd(Client&, std::vector<std::string> args);
		int operCmd(Client&, std::vector<std::string> args);


	public:
		Server(unsigned short port, const std::string &password);
		~Server();
		void run();
} ;
