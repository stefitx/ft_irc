#include "../inc/Server.hpp"

void	Server::executeCmd(Client &client, std::string cmd, std::vector<std::string> args)
{
	args.erase(args.begin());
	if (cmd == "NICK")
	{
		nickCmd(client, args);
		return ;
	}
	if (cmd == "USER")
	{
		userCmd(client, args);
		return ;
	}
	if (cmd == "PASS")
	{
		passCmd(client, args);
		return ;
	}
	// podriamos ponerle un codigo de retorno a los cmds y liego llamar a:
	// ServerReply(code, client);
}

void    Server::nickCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() == 0)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] NICK: not enough params" << std::endl;
		return;
	}
	std::map<int, Client*>::iterator	it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->getNick() == args[0])
		{
			// ERR_NICKNAMEINUSE (433) -> "<client> <nick> :Nickname is already in use"
			std::cerr << "[" << client.getFd() << "] NICK: sorry, nick already in use!\n";
			return;
		}
		it++;
	}
    client.setNick(args[0]);
}

void Server::passCmd(Client &client, std::vector<std::string> args)
{
	if (client.getRegistryState() == true)
	{
		// ERR_ALREADYREGISTERED (462) -> "<client> :You may not reregister"
		std::cerr << "[" << client.getFd() << "] PASS: stop it, you've already registered\n";
		return;
	}
	else if (!args.size())
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] PASS: empty password!\n";
		return;
	}
	else if (args[0] != _password)
	{
		// ERR_PASSWDMISMATCH (464) -> "<client> :Password incorrect"
		std::cerr << "[" << client.getFd() << "] PASS: wrong password...\n";
		return;
	}
	client.setRegistryState(true);
	std::cout << "Client fd " << client.getFd() << " authenticated successfully :)\n";
}

void Server::userCmd(Client &client, std::vector<std::string> args)
{
	if (args.size() < 4)
	{
		// ERR_NEEDMOREPARAMS (461) -> "<command> :Not enough parameters"
		std::cerr << "[" << client.getFd() << "] USER: Not enough params\n";
		return;
	}
	if (client.getHandShake() == true)
	{
		// ERR_ALREADYREGISTERED (462) -> "<client> :You may not reregister"
		std::cerr << "[" << client.getFd() << "] USER: already registered\n";
		return;
	}
	client.setUser(args[0]);
}