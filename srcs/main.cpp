/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atudor <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 15:26:39 by atudor            #+#    #+#             */
/*   Updated: 2025/07/08 20:05:17 by atudor           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <csignal>

static Server *g_server = NULL;

static void handleSignal(int)
{
    if (g_server)
        g_server->stop();
}

static unsigned short parsePort(const char *arg)
{
    char *end;
	long port;

	end = NULL;
    port = std::strtol(arg, &end, 10);
    if (*end != '\0' || port <= 0 || port > 65535)
		return (0);
    return (static_cast<unsigned short>(port));
}

int main(int argc, char **argv)
{
	unsigned short port;
	std::string pass;

    if (argc != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return EXIT_FAILURE;
    }

    port = parsePort(argv[1]);
    if (!port)
    {
        std::cerr << "Invalid port\n";
        return EXIT_FAILURE;
    }
    pass = argv[2];
    Server srv(port, pass);
	g_server = &srv;

    struct sigaction sa;
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    srv.run();
    return (0);
}
