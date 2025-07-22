/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atudor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:27:45 by atudor            #+#    #+#             */
/*   Updated: 2025/07/08 21:22:39 by atudor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include "../inc/Client.hpp"
#include "../inc/Channel.hpp"

Server::Server(unsigned short port, const std::string &password)
    : _port(port), _password(password), _listenFd(-1), _running(false)
{
    _operator_credentials["cris"]  = "mandarino";
    _operator_credentials["marta"] = "voley";
    _operator_credentials["stefi"] = "taylor";

}

Server::~Server()
{
	// Close all sockets and free Client / Channel objects
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
		if (it->fd >= 0)
			close(it->fd);

	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		delete it->second;
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
}

void Server::initListeningSocket()
{
	int opt;
	struct sockaddr_in addr;
	struct pollfd pfd;
	char  hostnameStr[256];

	if (gethostname(hostnameStr, sizeof(hostnameStr)) == -1)
		throw std::runtime_error("initConnection: gethostname()");
	_hostname = hostnameStr;
	_listenFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_listenFd == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	opt = 1;
	setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_port);

	if (bind(_listenFd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(_listenFd, 10) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	pfd.fd = _listenFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
}

void Server::acceptNewClient()
{
	// should we remove the while()?
	while (true)
	{
		struct sockaddr_in cliAddr;
		socklen_t len = sizeof(cliAddr);
		int fd = accept(_listenFd, reinterpret_cast<struct sockaddr *>(&cliAddr), &len);

		if (fd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break;
			perror("accept");
			return;
		}

		_clients[fd] = new Client(fd);
		struct pollfd pfd = {fd, POLLIN, 0};
		_pollFds.push_back(pfd);
		_clients[fd]->setConnectionTime(time(NULL));
		std::cout << "[+] Client connected fd=" << fd << "\n";
}
}

void Server::handleClientData(size_t idx)
{
	int fd = _pollFds[idx].fd;
	Client *c = _clients[fd];
	char buf[1024];

// 	while (true)
// {
		ssize_t bytes = recv(c->getFd(), buf, sizeof(buf), MSG_DONTWAIT);
		if (bytes == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;
			perror("recv");
			removeClient(idx);
			return;
		}
		if (bytes == 0)
		{
			std::cout << "[i] Client closed fd=" << fd << "\n";
			removeClient(idx);
			return;
		}
		else if (bytes > 1024)
		{
			std::cerr << "[!] Warning: Received more than 1024 bytes, weird things might happen ((>.<)).\n";
		}
		else
		{
			buf[bytes] = '\0';
			c->setBuffer(buf);
			if (c->getBuffer().find_first_of("\r\n") == std::string::npos)
				return;
			processBuffer(c); // Stub function for later parts
		}
	// }
}

void Server::removeClient(size_t idx)
{
	// int fd = _pollFds[idx].fd;
	// close(fd);
	// delete _clients[fd];
	// _clients.erase(fd);

	// _pollFds[idx] = _pollFds.back();
	// _pollFds.pop_back();
	int fd = _pollFds[idx].fd;

	if (_clients.count(fd)) {
		close(fd);
		delete _clients[fd];
		_clients.erase(fd);
	}
	if (idx != _pollFds.size() - 1)
		_pollFds[idx] = _pollFds.back();
	_pollFds.pop_back();
}

// Client *Server::lookupClientByFd(int fd)
// {
//     std::map<int, Client *>::iterator it = _clients.find(fd);
//     return (it == _clients.end()) ? NULL : it->second;
// }

// Channel *Server::getOrCreateChannel(const std::string &name)
// {
//     std::map<std::string, Channel *>::iterator it = _channels.find(name);
//     if (it != _channels.end()) return it->second;

//     Channel *ch = new Channel(name);
//     _channels[name] = ch;
//     return ch;
// }


void Server::processBuffer(Client *c)
{
	std::string &buf = c->getBuffer();
	std::vector<std::string> msgs;
	size_t pos;

	while ((pos = buf.find("\r\n")) != std::string::npos)
	{
		msgs.push_back(buf.substr(0, pos));
		buf.erase(0, pos + 2);
	}
	for (std::vector<std::string>::iterator it = msgs.begin(); it != msgs.end(); ++it)
	{
		std::istringstream iss(*it);
		std::string token;
		std::vector<std::string> args;

		while (iss >> token)
		{
			if (token[0] == ':')
			{
				std::string rest;
				std::getline(iss, rest);
				token += rest;
				if (!token.empty() && token[1] == ' ')
					token.erase(1, 1);
			}
			args.push_back(token);
		}
		if (!args.empty())
			executeCmd(*c, args[0], args);
	}
}


void Server::run()
{
	initListeningSocket();
	_running = true;
	std::cout << "[+] Listening on port " << _port << " (Linux)" << std::endl;

	while (_running)
	{
		if (_pollFds.empty()) {
			std::cerr << "ERROR: pollFds is empty! This should never happen.\n";
			break;
		}
		if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
		{
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}

		if (_pollFds[0].revents & POLLIN)
			acceptNewClient();

		for (ssize_t i = _pollFds.size() - 1; i >= 1; --i)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				handleClientData(i);
				continue;
			}

			if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				removeClient(i);
				continue;
			}
		}

		// while (i-- > 1)
		// {
		// 	// --i;
		// 	if (_pollFds[i].revents & POLLIN)
		// 	{
		// 		handleClientData(i);
		// 		continue;
		// 	}
		// 	if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
		// 	{
		// 		removeClient(i);
		// 		continue;
		// 	}
		// }
		std::cout << "Current poll fds:\n";
		for (size_t i = 0; i < _pollFds.size(); ++i)
			std::cout << "  [" << i << "] fd=" << _pollFds[i].fd << "\n";
	}
}
