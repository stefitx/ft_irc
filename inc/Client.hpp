/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atudor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:26:54 by atudor            #+#    #+#             */
/*   Updated: 2025/06/25 15:27:05 by atudor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <iostream>
# include "Channel.hpp"
# include "Server.hpp"

class	Client
{
	public:
		Client(int fd);
		Client(const Client&);
		~Client();

		Client	&operator=(const Client&);

		void					setNick(std::string nick);
		void					setUser(std::string user);
		void					setBuffer(std::string buf);
		void					setFd(int fd);
		void					setHost(int host);
		void					setHandShake(bool state);
		// void					setChannels(std::map<std::string, Channel> channels);
		void					setRegistryState(bool state);

		std::string				&getNick();
		std::string				&getUser();
		std::string				&getBuffer();
		// std::map<std::string, Channel>	getChannels() const;
		int						&getIp();
		int						&getFd();
		bool					getRegistryState();
		bool					getHandShake();
	private:
		std::string						nickName;
		std::string						userName;
		std::string						buffer;
		int								ip; // or hostname?
		int								fd;
		bool							registryState; // correct password, nick, user are set
		bool							handShake;
		// std::map<std::string, Channel>	channels;
};

