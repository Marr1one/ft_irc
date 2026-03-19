/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerNetwork.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 22:29:05 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/19 13:20:21 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"

void Server::acceptClient() {
	int client_fd = accept(_serverFd, NULL, NULL);
	if (client_fd < 0)
		return;
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0) {
		close(client_fd);
		return;
	}
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

	if (bytes < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cout << "Client " << fd << " disconnected\n";
		close(fd);
		removeClient(fd);
		return;
	}

	if (bytes == 0) {
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

void Server::flushPendingWrites(int fd) {
	ClientIt clientIt = _clients.find(fd);

	if (clientIt == _clients.end())
		return;
	if (!clientIt->second.hasPendingOutput()) {
		updatePollEvents(fd);
		return;
	}

	const std::string &buffer = clientIt->second.getOutputBuffer();
	ssize_t sent = send(fd, buffer.c_str(), buffer.size(), 0);

	if (sent < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		close(fd);
		removeClient(fd);
		return;
	}
	if (sent == 0)
		return;

	clientIt->second.consumeOutput(static_cast<size_t>(sent));
	updatePollEvents(fd);
}

void Server::removeClient(int fd) {
	removeClientFromPoll(fd);

	for (ChannelIt it = _channels.begin(); it != _channels.end(); ++it) {
		it->second.removeClient(fd);
		it->second.removeOperator(fd);
		it->second.removeInvited(fd);
	}

	_blackjackGames.erase(fd);
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

void Server::updatePollEvents(int fd) {
	for (PollFdIt it = _fds.begin(); it != _fds.end(); ++it) {
		if (it->fd == fd) {
			it->events = POLLIN;
			if (_clients.find(fd) != _clients.end() &&
				_clients[fd].hasPendingOutput())
				it->events |= POLLOUT;
			return;
		}
	}
}

void Server::sendChannelMsg(int fd, const std::string &channelName,
							const std::string &msg) {
	if (_channels.find(channelName) == _channels.end())
		std::cout << "Channel name not found!\n";
	else {
		std::vector<int> clients = _channels[channelName].getClientFds();
		for (std::vector<int>::iterator it = clients.begin();
			 it != clients.end(); ++it) {
			if (*it == fd)
				continue;
			if (_clients.find(*it) == _clients.end())
				continue;
			_clients[*it].queueOutput(msg);
			updatePollEvents(*it);
		}
	}
}

void Server::sendUserMsg(int fd, const std::string &target,
						 const std::string &msg) {
	(void)fd;
	for (ClientIt it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.get_nickname() == target) {
			it->second.queueOutput(msg);
			updatePollEvents(it->second.get_fd());
			return;
		}
	}
	std::cout << "User not found!\n";
}

void Server::sendReply(int fd, const std::string &reply) {
	std::string msg = reply + "\r\n";

	if (_clients.find(fd) == _clients.end())
		return;
	_clients[fd].queueOutput(msg);
	updatePollEvents(fd);
}
