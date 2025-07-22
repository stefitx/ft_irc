#include "../../inc/Server.hpp"


int Server::helpCmd(Client &client, std::vector<std::string> args)
{
	if(args.empty())
	{
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 704 " + client.getNick() + " index " + ": Commands Available:\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": NICK\t\tUSER\t\tPASS   \t\tQUIT\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": JOIN\t\tPART\t\tTOPIC  \t\tINVITE\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + " index " + ": KICK\t\tMODE\t\tPRIVMSG\t\tOPER\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 706 " + client.getNick() + " index " + ": Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
	}
	else if (args[0] == "-l")
	{
		sendLine(client, "\r\n");
		sendLine(client, " Commands Available:\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t NICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: NICK <nickname>, sets your nick\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t USER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: USER <username> <hostname> <servername> :<realname>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PASS" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PASS <password>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t QUIT" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: QUIT [<reason>], disconnects from the current server\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t JOIN" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: JOIN <channel>, joins the channel\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PART" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t TOPIC" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t INVITE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t KICK" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: KICK <nick> [reason], kicks the nick from the current channel (needs chanop)\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t MODE" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: MODE <channel> [<mode>]\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t PRIVMSG" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: PRIVMSG <target> :<message>\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + "\t OPER" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")) + " : Usage: OPER <username> <password>, grants operator privileges\r\n");
		sendLine(client, "\r\n");
		sendLine(client, " Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
	}
	else
	{
		sendLine(client, "\r\n");
		sendLine(client, (client.getIsNetCat() ? std::string(RED) : std::string("\00304")) + " Usage\r\n" + (client.getIsNetCat() ? std::string(RESET) : std::string("\017")));

		if (args[0] == "NICK" || args[0] == "nick")
			sendLine(client, "\t\tNICK <nickname>, sets your nick\r\n");
		else if (args[0] == "USER" || args[0] == "user")
			sendLine(client, "\t\tUSER <username> <hostname> <servername> :<realname>\r\n");
		else if (args[0] == "PASS" || args[0] == "pass")
			sendLine(client, "\t\tPASS <password>\r\n");
		else if (args[0] == "QUIT" || args[0] == "quit")
			sendLine(client, "\t\tQUIT [<reason>], disconnects from the current server\r\n");
		else if (args[0] == "JOIN" || args[0] == "join")
			sendLine(client, "\t\tJOIN <channel>, joins the channel\r\n");
		else if (args[0] == "PART" || args[0] == "part")
			sendLine(client, "\t\tPART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		else if (args[0] == "TOPIC" || args[0] == "topic")
			sendLine(client, "\t\tTOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		else if (args[0] == "INVITE" || args[0] == "invite")
			sendLine(client, "\t\tINVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		else if (args[0] == "KICK" || args[0] == "kick")
			sendLine(client, "\t\tKICK <nick> [reason], kicks the nick from the current channel (needs chanop)\r\n");
		else if (args[0] == "MODE" || args[0] == "mode")
			sendLine(client, "\t\tMODE <channel> [<mode>]\r\n");
		else if (args[0] == "PRIVMSG" || args[0] == "privmsg")
			sendLine(client, "\t\tPRIVMSG <target> :<message>\r\n");
		else if (args[0] == "OPER" || args[0] == "oper")
			sendLine(client, "\t\tOPER <username> <password>, grants operator privileges\r\n");
		else
		{
			// ERR_HELPNOTFOUND (524) -> "client> <subject> :No help available on this topic"
			std::cerr << "[" << client.getFd() << "] HELP: No help available on this topic\n";
			return (524);
		}
	}
	return (0);
}