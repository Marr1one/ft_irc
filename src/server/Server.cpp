/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 21:54:37 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 14:57:47 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool Server::_running = true;

Server::Server(char *port, char *password)
	: _port(0), _password(password), _serverFd(-1) {
	char *end = NULL;

	errno = 0;
	long value = std::strtol(port, &end, 10);

	if (port == NULL || *port == '\0' || end == port || *end != '\0' ||
		errno == ERANGE || value < 1 || value > 65535) {
		throw std::runtime_error("Invalid port value");
	}

	_port = static_cast<int>(value);
	initCommandHandlers();
}

Server::~Server() {}

void Server::signalHandler(int signal) {
	(void)signal;
	_running = false;
}

int Server::findClientByNick(const std::string &nick) const {
	for (ConstClientIt it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.get_nickname() == nick)
			return it->first;
	}
	return -1;
}

bool Server::checkNickname(const std::string &nickname) const {
    for(ConstClientIt it = _clients.begin(); it != _clients.end();it++)
    {
        if(it->second.get_nickname() == nickname)
            return (false);
    }
    return (true);
}

bool Server::checkUsername(const std::string &username) const {
    for (ConstClientIt it = _clients.begin();
         it != _clients.end(); it++)
    {
        if (it->second.get_username() == username)
            return (false);
    }
    return (true);
}

bool Server::canFinishRegistration(const Client &client) const
{
    return (client.is_registerable());
}

void Server::joinChannel(int fd, const std::string &name)
{
	bool isNew = (_channels.find(name) == _channels.end());

	if (isNew)
		_channels[name] = Channel(name);

	Client *client = &_clients[fd];
	_channels[name].addClient(client);
	if (isNew)
		_channels[name].addOperator(fd);

	std::cout << "Client " << fd << " joined channel : " << name;
	if (isNew)
		std::cout << " (created, operator)";
	std::cout << std::endl;
}

void Server::partChannel(int fd, const std::string &name)
{
	ChannelIt it = _channels.find(name);

	if (it == _channels.end())
		return;

	it->second.removeClient(fd);
	it->second.removeOperator(fd);
	it->second.removeInvited(fd);
}
