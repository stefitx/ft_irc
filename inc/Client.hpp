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
# include "Server.hpp"

class Channel;

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
		void					setHost(std::string host);
		void					setHandShake(bool state);
		void					setChannelsJoined(int channelsJoined);
		void					setChannels(std::map<std::string, Channel*> channels);
		void					setRegistryState(bool state);
		void					setServerOper(bool state);
		void					setConnectionTime(time_t time);
		void					setIsNetCat(bool state);
		void					setAddr(struct sockaddr_in addr);


		std::string				&getNick();
		std::string				&getUser();
		std::string				&getBuffer();
		int						&getChannelsJoined();
		Channel*				isChannelMember(std::string channelName);
		std::map<std::string, Channel *>	&getChannels();
		std::string				&getIp();
		int						&getFd();
		struct sockaddr_in		&getAddr(void);
		bool					getRegistryState();
		bool					getHandShake();
		bool					getServerOper();
		time_t					getConnectionTime() const;
		bool					getIsNetCat() const;
		void					addJoinedChannel(Channel *joinedChannel);
		void					removeJoinedChannel(Channel *joinedChannel);
		std::string &inBuf();
    	const std::string &inBuf() const;

	private:
		std::string						_nickName;
		std::string						_userName;
		std::string						_buffer;
		std::string						_host; // or hostname?
		int								_fd;
		bool							_registryState; // correct password, nick, user are set
		bool							_handShake;
		bool							_isChannelOper;
		bool							_isServerOper;
		time_t							_connectionTime;
		bool							_isNetCat;
		int								_channelsJoined;
		struct sockaddr_in				_addr;
		std::map<std::string, Channel *>	_channels;
		std::string _inBuf; 
};

