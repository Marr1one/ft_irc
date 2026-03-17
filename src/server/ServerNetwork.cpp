/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerNetwork.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 22:29:05 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 14:06:17 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Parser.hpp"

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
