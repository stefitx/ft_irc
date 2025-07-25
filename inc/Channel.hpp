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

		void	setMode(const std::string mode);
		void	setPasswordMode(bool status);
		void	setInviteMode(bool status);
		void	setTopicRestrictionMode(bool status);
		void	setUserLimitMode(bool status);
		
		void	addIvitedUser(Client *client);
		void	setTopic(const std::string newTopic);
		void	setPassword(const std::string key);
		void	setTopicSetterMember(Client *memeber);
		void	setTopicSetTime(std::string time);
		void	setCreationTime(std::string time);

		int		authorizedToJoin(Client* client, std::string key);

		Client*		getMembers(std::string clientName);
		std::string	getTopic();
		Client*		getOperators(std::string name);
		bool		getTopicRestrictionMode();
		Client*		getTopicSetterMember();
		std::string	getTopicSetTime();
		std::string	getCreationTime();
		bool		isMember(Client *c) const;
		bool 		isOperator(Client *c) const;

		bool		isChannelOperator(std::string memberName);
		bool		isInvitedUser(Client *userName);
		bool		isInviteMode();

		std::map<std::string, Client *>	&getMapMembers();
		std::map<std::string, Client *>	&getMapOperators();
		const std::string	&getMode()          const { return (_mode);     }

		bool	hasMode(char c)	const;
		void	addMode(char c);
		void	removeMode(char c);
		int		getUserLimit()	const;
		void	setUserLimit(int n);
		const std::string	&getPassword() const;


	private:
		std::string					_name;
		std::string					_password;
		int							_clientNum;
		std::string					_topic;
		int							_userLimit;
		std::vector<Client *>		_invitedUsers;

		std::string					_mode;
		bool						_passwordMode;
		bool						_inviteMode;
		bool						_topicRestrictionMode;
		bool						_userLimitMode;
		Client*						_topicSetterMember;
		std::string					_topicSetTime;
		std::string					_creationTime;

		std::map<std::string, Client *>		_members;
		std::map<std::string, Client *>		_operators;
};
