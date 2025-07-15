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

Server::Server(unsigned short port, const std::string &password)
    : _port(port), _password(password), _listenFd(-1), _running(false) {}

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

void Server::makeSocketNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void Server::initListeningSocket()
{
	int					opt;
	struct sockaddr_in	addr;
	struct pollfd		pfd;

    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    opt = 1;
    setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(_port);

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

    makeSocketNonBlocking(_listenFd);

    pfd.fd = _listenFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);
}

void Server::acceptNewClient()
{
    while (true)
    {
        struct sockaddr_in cliAddr; socklen_t len = sizeof(cliAddr);
        int fd = accept(_listenFd, reinterpret_cast<struct sockaddr *>(&cliAddr), &len);

        if (fd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break; // No more
            perror("accept");
            break;
        }

        makeSocketNonBlocking(fd);
        _clients[fd] = new Client(fd);

        struct pollfd pfd = { fd, POLLIN, 0 };
        _pollFds.push_back(pfd);

        std::cout << "[+] Client connected fd=" << fd << "\n";
    }
}

void Server::handleClientData(size_t idx)
{
    int  fd = _pollFds[idx].fd;
    Client *c = new Client(fd);
    char buf[512];

    while (true)
    {
        ssize_t bytes = recv(c->getFd(), buf, sizeof(buf), 0);
        if (bytes == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
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
        else if(bytes > 512)
        {
            std::cerr << "[!] Warning: Received more than 512 bytes, weird things might happen ((>.<)).\n";
        }
        else
        {
            buf[bytes] = '\0';
            c->setBuffer(buf);
            if (c->getBuffer().find_first_of("\r\n") == std::string::npos)
                return;
            processBuffer(c); // Stub function for later parts
        }
    }
}

void Server::removeClient(size_t idx)
{
    int fd = _pollFds[idx].fd;
    close(fd);
    delete _clients[fd];
    _clients.erase(fd);

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
    // TODO: In Part 2 we will parse and dispatch commands here.
    (void)c;
    std::cout << "[>] RECV line: " << c->getBuffer();
}

void Server::run()
{
    initListeningSocket();
    _running = true;
    std::cout << "[+] Listening on port " << _port << " (Linux)" << std::endl;

    while (_running)
    {  
      //  std::cout << "inside running loop " << std::endl;
        if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
        {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }

        if (_pollFds[0].revents & POLLIN) acceptNewClient();

        size_t i = _pollFds.size();
        while (i > 1)
        {
            --i;
            if (_pollFds[i].revents & POLLIN) handleClientData(i);
            if (_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) removeClient(i);
        }
    }
}
