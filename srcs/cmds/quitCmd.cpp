#include "../../inc/Server.hpp"

void Server::disconnectClient(Client &client, std::string message)
{
	int fd = client.getFd();
	std::cout << "Disconnecting client fd " << fd << "...\n";

	std::map<std::string, Channel *>& channels = client.getChannels();
	for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); ++it) {
		Channel* chan = getChannel(it->first);
		if (chan) {
			chan->removeMember(&client);
			std::string quit_line = ":" + client.getNick() + "!~" + client.getUser() + "@" + client.getIp() + " QUIT " + (message.empty() ? "Client Quit" : message);
			chan->broadcast(quit_line, client);
			if (&client == chan->getTopicSetterMember())
				chan->setTopicSetterMember(NULL);
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
	disconnectClient(client, message);
	return (0);
}
