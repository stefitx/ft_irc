#include "../inc/Channel.hpp"

Channel::Channel(std::string name) {
	this->name = name;
	clientNum = 0;
	userLimit = 0;
	topic = "";
	password = "";
	privileges.clear();
	mode.clear();
	accessType = "";
}

Channel::~Channel() {}

Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		name = other.name;
		clientNum = other.clientNum;
		userLimit = other.userLimit;
		topic = other.topic;
		password = other.password;
		privileges = other.privileges;
		mode = other.mode;
		accessType = other.accessType;
	}
	return *this;
}
