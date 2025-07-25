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

#include "Client.hpp"
#include "Channel.hpp"

//Condiciones generales del server (RPL_ISUPPORT)
# define CHANNELMODES "itkol kol" //CHANMODES en orden segun la sintaxis de la rply 004 (RPL_MYINFO) segun el subject

//ISUPPORT Parameter Macros
# define CHANLIMIT "#:4" 
# define CHANMODES ",ko,l,it" // Define los modos de canal soportados y cómo se clasifican segun el subject (obligatorio).
# define CHANNELLEN "42" // Los nombres de canal pueden tener hasta CHANNELLEN caracteres.
# define CHANTYPES "#" // Los nombres de canal deben comenzar con algun caracter de CHANTYPES.
# define MAXCONN "128"
# define KICKLEN "369" //El parametro <reason> del comando KICK puede tener hasta KICKLEN caracteres
# define MODES "3" //Se pueden cambiar hasta 3 modos de tipo A|B|C (parametro requerido) en un solo comando MODE.
# define NICKLEN "30" //Los nicks pueden tener hasta NICKLEN caracteres.
# define PREFIX "(o)@" // Define los roles en los canales (en este caso, solo operadores).
# define TARGMAX "JOIN:4"
# define TOPICLEN "307" //Los tópicos pueden tener hasta TOPICLEN caracteres.
# define USERLEN "12"

//ISUPPORT Parameter Macros as Integers
# define CHANLIMITNUM 4
# define CHANNELLENNBR 42
# define KICKLENNUM 369
# define NICKLENNBR 30
# define TOPICLENNBR 307
# define USERLENNUM 12

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

		void		createChannel(std::string channelName, Client *client);
		void pruneChannel(Channel* chan);
		bool		reply(Client &cli, int code, const std::string &params, const std::string &text);
		void		executeCmd(Client &, std::string cmd, std::vector<std::string> args);
		void		handshake(Client &client);
		CommandType	isCommand(const std::string &cmd);
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
} ;
bool sendLine(Client &cli, const std::string &line);
