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
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include "Colors.h"
#include "Client.hpp"
#include "Channel.hpp"
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

 		void acceptNewClient();
		void handleClientData(size_t pollIndex);
		void removeClient(size_t pollIndex);


		// Client *lookupClientByFd(int fd);
		void    processBuffer(Client *c);
		// Channel *getOrCreateChannel(const std::string &name);

		bool sendLine(Client &cli, const std::string &line);
		bool reply(Client &cli, int code, const std::string &params, const std::string &text);
		void	executeCmd(Client &, std::string cmd, std::vector<std::string> args);
		void	handshake(Client &client);
		CommandType isCommand(const std::string &cmd);
		Channel *get_channel(const std::pair<std::string, Channel> &pair);
		void	disconnectClient(Client &client);
		void	errorReply(Client &cli, int code, std::string cmd);

		// COMMANDS TO BE RECEIVED
		int	nickCmd(Client&, std::vector<std::string> args);
		int	userCmd(Client&, std::vector<std::string> args);
		int	passCmd(Client&, std::vector<std::string> args);
		std::map<std::string, std::string>	*parseJoinArgs(std::vector<std::string> args);
		int	joinCmd(Client&, std::vector<std::string> args);
		int	helpCmd(Client&, std::vector<std::string> args);
		int operCmd(Client&, std::vector<std::string> args);
		int dieCmd(Client&, std::vector<std::string> args);
		int quitCmd(Client&, std::vector<std::string> args);


	public:
		Server(unsigned short port, const std::string &password);
		~Server();
		void run();
} ;
