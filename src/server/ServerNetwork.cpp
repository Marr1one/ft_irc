/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerNetwork.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 22:29:05 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 16:30:23 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"

void Server::acceptClient() {
	int client_fd = accept(_serverFd, NULL, NULL);
	if (client_fd < 0)
		return; // ptet ajouter un message derreur
	pollfd newClient;
	newClient.fd = client_fd;
	newClient.events = POLLIN;
	newClient.revents = 0;
	_fds.push_back(newClient);
	_clients[client_fd] = Client(client_fd);
	std::cout << "New client connected! fd : " << client_fd << std::endl;
}

void Server::receiveMessage(int i) {
	char buffer[1024];
	int fd = _fds[i].fd;
	int bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0) {
		std::cout << "Client " << fd << " disconnected\n";
		close(fd);
		removeClient(fd);
		return;
	}

	buffer[bytes] = '\0';
	_clients[fd].appendBuffer(buffer);

	while (_clients[fd].hasLine()) {
		std::string line = _clients[fd].extractLine();
		if (line.empty())
			continue;

		Message msg;
		if (!Parser::parse(line, msg))
			continue;

		handleMessage(fd, msg);
	}
}

void Server::removeClient(int fd) {
	removeClientFromPoll(fd);

	for (ChannelIt it = _channels.begin(); it != _channels.end(); ++it) {
		it->second.removeClient(fd);
		it->second.removeOperator(fd);
		it->second.removeInvited(fd);
	}

	_clients.erase(fd);
}

void Server::removeClientFromPoll(int fd) {
	for (PollFdIt it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == fd) {
			_fds.erase(it);
			return;
		}
	}
}

void Server::sendChannelMsg(int fd, const std::string &channelName,
							const std::string &msg) {
	if (_channels.find(channelName) == _channels.end())
		std::cout << "Channel name not found!\n";
	else
		_channels[channelName].broadcast(fd, msg);
}

void Server::sendUserMsg(int fd, const std::string &target,
						 const std::string &msg) {
	(void)fd;
	for (ClientIt it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.get_nickname() == target) {
			send(it->second.get_fd(), msg.c_str(), msg.size(), 0);
			return;
		}
	}
	std::cout << "User not found!\n";
}

void Server::sendReply(int fd, const std::string &reply) {
	std::string msg = reply + "\r\n";
	send(fd, msg.c_str(), msg.size(), 0);
}