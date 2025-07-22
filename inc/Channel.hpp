#pragma once

# include <iostream>
# include <vector>
# include <map>
# include "Client.hpp"

class Client;

class	Channel
{
	public:
		Channel(std::string name);
		Channel(const Channel&);
		~Channel();

		Channel	&operator=(const Channel&);

		void	broadcast(std::string msg, Client &client);
		void	addMember(Client *client);
		void	addOperator(Client *client);
		void	removeMember(Client* client);
		void	removeOperator(Client* client);
		std::string	getName() const;

		void	setChanOperator(Client *creator);
		void	setPassword(const std::string key);
		void	setMode(const std::string mode);

		int		authorizedToJoin(Client* client, std::string key);

		Client*		getMembers(std::string clientName);
		std::string	getTopic();

		std::map<std::string, Client *>	&getMapMembers();

	private:
		std::string					_name;
		std::string					_password;
		int							_clientNum;
		std::string					_topic;
		int							_userLimit;
		std::vector<std::string>	_invitedMembers;

		std::string					_mode;
		bool						_passwordMode;
		bool						_inviteMode;
		bool						_topicRestrictionMode;
		bool						_operPrivilegeMode;
		bool						_userLimitMode;

		Client*						_chanOperator; // the first client to create the channel
		std::string					_accessType; // muy relacionado con 'mode'

		std::map<std::string, Client *>		_members; // map of clients in the channel
		std::map<std::string, Client *>		_operators;
};
