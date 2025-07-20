/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atudor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:27:37 by atudor            #+#    #+#             */
/*   Updated: 2025/06/25 15:27:39 by atudor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Client.hpp"

Client::Client(int fd)
{
	nickName = "";
	userName = "";
	buffer = "";
	ip = 0;
	this->fd = fd;
	registryState = false;
	handShake = false;
	isChannelOper = false;
	isServerOper = false;
	_isNetCat = false;
	channels = std::map<std::string, Channel>();
}

Client::Client(const Client &other) :
	nickName(other.nickName),
	userName(other.userName),
	buffer(other.buffer),
	ip(other.ip),
	fd(other.fd),
	registryState(other.registryState),

	handShake(other.handShake),
	isChannelOper(other.isChannelOper),
	isServerOper(other.isServerOper),
	connectionTime(other.connectionTime),
	_isNetCat(other._isNetCat),
	channels(other.channels)
{}

Client::~Client() {}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		fd = other.fd;
		registryState = other.registryState;
		handShake = other.handShake;
		nickName = other.nickName;
		userName = other.userName;
		buffer = other.buffer;
		ip = other.ip;
		isServerOper = other.isServerOper;
		isChannelOper = other.isChannelOper;
		connectionTime = other.connectionTime;
		_isNetCat = other._isNetCat;
		channels = other.channels;
	}
	return *this;
}

std::string &Client::getNick() { return nickName; }
std::string &Client::getUser() { return userName; }
std::string &Client::getBuffer() { return buffer; }
int &Client::getIp() { return ip; }
 std::map<std::string, Channel> Client::getChannels() const { return channels; }
int &Client::getFd() { return fd; }
bool Client::getRegistryState() { return registryState; }
bool Client::getHandShake() { return handShake; }
bool Client::getServerOper() { return isServerOper; }
time_t Client::getConnectionTime() const { return connectionTime; }
bool Client::getIsNetCat() const { return _isNetCat; }

void Client::setNick(std::string nick) { nickName = nick; }
void Client::setUser(std::string user) { userName = user; }
void Client::setBuffer(std::string buf) { buffer += buf; }
void Client::setFd(int fd) { this->fd = fd; }
void Client::setHost(int host) { ip = host; }
void Client::setHandShake(bool state) { handShake = state; }
void Client::setChannels(std::map<std::string, Channel> channels) { this->channels = channels; }
void Client::setRegistryState(bool state) { registryState = state; }
void Client::setServerOper(bool state) { isServerOper = state; }
void Client::setConnectionTime(time_t time) { connectionTime = time; }
void Client::setIsNetCat(bool state) { _isNetCat = state; }
