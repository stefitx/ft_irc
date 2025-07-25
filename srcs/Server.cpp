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
	struct sockaddr_in cliAddr;
	socklen_t len = sizeof(cliAddr);
	int fd = accept(_listenFd, reinterpret_cast<struct sockaddr *>(&cliAddr), &len);

	if (fd == -1)
	{
		throw std::runtime_error("initServer: accept()");
		// if (errno == EAGAIN || errno == EWOULDBLOCK)
		// {
		// 	std::cout << "No more clients to accept at the moment." << std::endl;
		// 	break;
		// }
		// perror("accept");
		// return;
	}

	_clients[fd] = new Client(fd);
	struct pollfd pfd = {fd, POLLIN, 0};
	_clients[fd]->setAddr(cliAddr);
	_clients[fd]->getAddr().sin_family = AF_INET;
	_clients[fd]->getAddr().sin_port = htons(_port); // Port is not set here, it will be set later
	_pollFds.push_back(pfd);
	_clients[fd]->setConnectionTime(time(NULL));
}

void Server::handleClientData(std::size_t idx)
{
    int     fd = _pollFds[idx].fd;
    Client *c  = _clients[fd];
    char    buf[1024];
    ssize_t n = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);

    if (n > 0)
    {
        c->getBuffer().append(buf, static_cast<std::size_t>(n));
        processBuffer(c);
        return;
    }
    if (n == 0)
    {
        if (!c->getBuffer().empty())
        {
            c->getBuffer().append("\r\n");
            processBuffer(c);
        }
        removeClient(idx);
        return;
    }
    if (errno == EAGAIN || errno == EWOULDBLOCK)
        return;
    if (errno == ECONNRESET)
    {
        if (!c->getBuffer().empty())
        {
            c->getBuffer().append("\r\n");
            processBuffer(c);
	     }
        removeClient(idx);
        return;
    }
    perror("recv");
    removeClient(idx);
}


void Server::removeClient(size_t idx)
{
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
		{
			executeCmd(*c, args[0], args);
		}
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

		// std::cout << "Current poll fds:\n";
		// for (size_t i = 0; i < _pollFds.size(); ++i)
		// 	std::cout << "  [" << i << "] fd=" << _pollFds[i].fd << "\n";
	}
}

void Server::stop()
{
        _running = false;
}

void Server::pruneChannel(Channel* chan)
{
    if (chan && chan->getMapMembers().empty())
    {
        const std::string name = chan->getName();
        _channels.erase(name);
        delete chan;
    }
}
