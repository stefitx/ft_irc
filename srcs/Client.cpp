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
	_nickName = "";
	_userName = "";
	_buffer = "";
	_host = "";
	_fd = fd;
	_registryState = false;
	_handShake = false;
	_isChannelOper = false;
	_isServerOper = false;
	_isNetCat = false;
	_channelsJoined = 0;
	memset(&_addr, 0, sizeof(_addr));
	_channels = std::map<std::string, Channel *>();
}

Client::Client(const Client &other) :
	_nickName(other._nickName),
	_userName(other._userName),
	_buffer(other._buffer),
	_host(other._host),
	_fd(other._fd),
	_registryState(other._registryState),

	_handShake(other._handShake),
	_isChannelOper(other._isChannelOper),
	_isServerOper(other._isServerOper),
	_connectionTime(other._connectionTime),
	_isNetCat(other._isNetCat),
	_channelsJoined(other._channelsJoined),
	_addr(other._addr),
	_channels(other._channels)
{}

Client::~Client() {}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_registryState = other._registryState;
		_handShake = other._handShake;
		_nickName = other._nickName;
		_userName = other._userName;
		_buffer = other._buffer;
		_host = other._host;
		_isServerOper = other._isServerOper;
		_isChannelOper = other._isChannelOper;
		_connectionTime = other._connectionTime;
		_isNetCat = other._isNetCat;
		_channelsJoined = other._channelsJoined;
		_addr = other._addr;
		_channels = other._channels;
	}
	return *this;
}

std::string &Client::getNick() { return _nickName; }
std::string &Client::getUser() { return _userName; }
std::string &Client::getBuffer() { return _buffer; }
std::string &Client::getIp() { return _host; }
std::map<std::string, Channel *> &Client::getChannels() { return _channels; }
int &Client::getFd() { return _fd; }
bool Client::getRegistryState() { return _registryState; }
bool Client::getHandShake() { return _handShake; }
bool Client::getServerOper() { return _isServerOper; }
int &Client::getChannelsJoined() { return _channelsJoined; }
time_t Client::getConnectionTime() const { return _connectionTime; }
bool Client::getIsNetCat() const { return _isNetCat; }
struct sockaddr_in &Client::getAddr(void) { return (_addr); }

void Client::setNick(std::string nick) { _nickName = nick; }
void Client::setUser(std::string user) { _userName = user; }
void Client::setBuffer(std::string buf) { _buffer += buf; }
void Client::setFd(int fd) { _fd = fd; }
void Client::setHost(std::string host) { _host = host; }
void Client::setHandShake(bool state) { _handShake = state; }
void Client::setChannels(std::map<std::string, Channel*> _channels) { this->_channels = _channels; }
void Client::setRegistryState(bool state) { _registryState = state; }
void Client::setServerOper(bool state) { _isServerOper = state; }
void Client::setConnectionTime(time_t time) { _connectionTime = time; }
void Client::setIsNetCat(bool state) { _isNetCat = state; }
void Client::setAddr(struct sockaddr_in addr) { _addr = addr; }


void	Client::addJoinedChannel(Channel *joinedChannel)
{
	_channels.insert(std::pair<std::string, Channel *>(joinedChannel->getName(), joinedChannel));
	_channelsJoined++;
}

void	Client::removeJoinedChannel(Channel *joinedChannel)
{
	_channels.erase(joinedChannel->getName());
	_channelsJoined--;
}
