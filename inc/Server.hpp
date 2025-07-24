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


# define CHANLIMITNUM 4

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
	DIE,
	KILL,
	NOTICE,
	UNKNOWN
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

		void	initListeningSocket();
 		void	acceptNewClient();
		void	handleClientData(size_t pollIndex);
		void	removeClient(size_t pollIndex);

		void		processBuffer(Client *c);
		Channel		*getChannel(const std::string &name);

		void		createChannel(std::string channelName, std::string key, Client *client);
		void pruneChannel(Channel* chan);
		bool		reply(Client &cli, int code, const std::string &params, const std::string &text);
		void		executeCmd(Client &, std::string cmd, std::vector<std::string> args);
		void		handshake(Client &client);
		CommandType	isCommand(const std::string &cmd);
		//Channel *get_channel(const std::pair<std::string, Channel> &pair);
		void		disconnectClient(Client &client);
		void		errorReply(Client &cli, int code, std::string cmd, std::vector<std::string> args);
		std::string	itoa3(int code);
		std::map<std::string, std::string>	*parseJoinArgs(std::vector<std::string> args);
		std::vector<std::string> vectorSplit(const std::string& str, char delim);
		std::string colorLine(Client &client, const std::string &line, const std::string &color);

		// COMMANDS TO BE RECEIVED
		int	nickCmd(Client&, std::vector<std::string> args);
		int	userCmd(Client&, std::vector<std::string> args);
		int	passCmd(Client&, std::vector<std::string> args);
		int	joinCmd(Client&, std::vector<std::string> args);
		int	helpCmd(Client&, std::vector<std::string> args);
		int operCmd(Client&, std::vector<std::string> args);
		int quitCmd(Client&, std::vector<std::string> args);
		int killCmd(Client&, std::vector<std::string> args);
		int privmsgCmd(Client&, std::vector<std::string> args);
		int kickCmd(Client &, std::vector<std::string> args);
		int inviteCmd(Client &, std::vector<std::string> args);
		void noticeCmd(Client &, std::vector<std::string> args);
		int partCmd(Client &client, std::vector<std::string> args);

		int	topicCmd(Client &, std::vector<std::string> args);
		int dieCmd(Client&);
		int modeCmd(Client&, std::vector<std::string> args);
		int modeInitialChecks(Client &client, std::vector<std::string> args, const std::string &chanName, Channel *chan);

	public:
		Server(unsigned short port, const std::string &password);
		~Server();
		void	run();
		void    stop();
	//	bool sendLine(Client &cli, const std::string &line);

} ;
bool sendLine(Client &cli, const std::string &line);
