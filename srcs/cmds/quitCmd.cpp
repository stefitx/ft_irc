#include "../../inc/Server.hpp"

void Server::disconnectClient(Client &client)
{
	int fd = client.getFd();
	std::cout << "Disconnecting client fd " << fd << "...\n";

	std::map<std::string, Channel *>& channels = client.getChannels();
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* chan = getChannel(it->first);
		if (chan) {
			chan->removeMember(&client);
		}
	}
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == fd) {
			std::cout << "Removing client fd " << fd << " from pollFds...\n";
			removeClient(i);
			break;
		}
	}
	std::cout << "Client fd " << fd << " disconnected successfully.\n";
}

int Server::quitCmd(Client &client, std::vector<std::string> args)
{
	std::string message;
	for (size_t i = 0; i < args.size(); ++i)
	{
		if (i > 1)
			message += " ";
		message += args[i];
	}
	std::cout << "Client " << client.getNick() << " is quitting with message: " << message << "\n";
	std::map<std::string, Channel *>::iterator it;
	for (it = client.getChannels().begin(); it != client.getChannels().end(); ++it) {
        Channel* chan = getChannel(it->first);
        if (chan) {
			std::string quit_line = ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " QUIT " + (args.empty() ? "Client Quit" : args[0]);
			chan->broadcast(quit_line, client);
            chan->removeMember(&client);
        }
    }
	disconnectClient(client);
	return (0);
}
