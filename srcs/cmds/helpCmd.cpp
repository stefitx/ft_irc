#include "../../inc/Server.hpp"

int Server::helpCmd(Client &client, std::vector<std::string> args)
{
	if(args.empty())
	{
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 704 " + client.getNick() + ": Commands Available:\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + ": NICK\t\tUSER\t\tPASS   \t\tQUIT\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + ": JOIN\t\tPART\t\tTOPIC  \t\tINVITE\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + ": KICK\t\tMODE\t\tPRIVMSG\t\tOPER\r\n");
		sendLine(client, ":" + _hostname + " 705 " + client.getNick() + ": WELP\t\tDIE\t\tKILL   \t\tNOTICE\r\n");
		sendLine(client, "\r\n");
		sendLine(client, ":" + _hostname + " 706 " + client.getNick() + ": Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
	}
	else if (args[0] == "-l")
	{
		sendLine(client, "\r\n");
		sendLine(client, " Commands Available:\r\n");
		sendLine(client, colorLine(client, "\t NICK", "RED") + ": Usage: NICK <nickname>, sets your nick\r\n");
		sendLine(client, colorLine(client, "\t USER", "RED") + ": Usage: USER <username> <hostname> <servername> :<realname>\r\n");
		sendLine(client, colorLine(client, "\t PASS", "RED") + ": Usage: PASS <password>\r\n");
		sendLine(client, colorLine(client, "\t QUIT", "RED") + ": Usage: QUIT [<reason>], disconnects from the current server\r\n");
		sendLine(client, colorLine(client, "\t JOIN", "RED") + ": Usage: JOIN <channel>[,<channel>] joins the channel\r\n");
		sendLine(client, colorLine(client, "\t PART", "RED") + ": Usage: PART [<channel>] [<reason>], leaves the channel, by default the current one\r\n");
		sendLine(client, colorLine(client, "\t TOPIC", "RED") + ": Usage: TOPIC [<topic>], sets the topic if one is given, else shows the current topic\r\n");
		sendLine(client, colorLine(client, "\t INVITE", "RED") + ": Usage: INVITE <nick> [<channel>], invites someone to a channel, by default the current channel (needs chanop)\r\n");
		sendLine(client, colorLine(client, "\t KICK", "RED") + ": Usage:  Usage: KICK <channel> <user>[,user2...] [reason]\r\n");
		sendLine(client, colorLine(client, "\t MODE", "RED") + ": Usage: MODE <channel> [<mode>]\r\n");
		sendLine(client, colorLine(client, "\t PRIVMSG", "RED") + ": Usage: PRIVMSG <target>[,<target>] <text to be sent>, sends private messages between users and channels\r\n");
		sendLine(client, colorLine(client, "\t OPER", "RED") + ": Usage: OPER <username> <password>, grants operator privileges\r\n");
		sendLine(client, colorLine(client, "\t DIE", "RED") + ": Usage: DIE, instructs the server to shut down\r\n");
		sendLine(client, colorLine(client, "\t KILL", "RED") + ": Usage: KILL <nickname> <comment>, disconnects target from the current server\r\n");
		sendLine(client, colorLine(client, "\t NOTICE", "RED") + ": Usage: NOTICE <target>[,<target>] <text to be sent>, sends notices between users and channels\r\n");
		sendLine(client, "\r\n");
		sendLine(client, " Type /WELP <command> for more information, or /WELP -l\r\n");
		sendLine(client, "\r\n");
	}
	else
	{
		sendLine(client, "\r\n");
		sendLine(client, colorLine(client, "Usage", "RED") + "\r\n");

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
			sendLine(client, "\t\tPRIVMSG <target>[,<target>] <text to be sent>, sends private messages between users and channels\r\n");
		else if (args[0] == "OPER" || args[0] == "oper")
			sendLine(client, "\t\tOPER <username> <password>, grants operator privileges\r\n");
		else if (args[0] == "DIE" || args[0] == "die")
			sendLine(client, "\t\tDIE, instructs the server to shut down\r\n");
		else if (args[0] == "KILL" || args[0] == "kill")
			sendLine(client, "\t\tKILL <nickname> <comment>, disconnects target from the current server\r\n");
		else if (args[0] == "NOTICE" || args[0] == "notice")
			sendLine(client, "\t\tNOTICE <target>[,<target>] <text to be sent>, sends notices between users and channels\r\n");
		else
			return (524); // ERR_HELPNOTFOUND (524)
	}
	return (0);
}
