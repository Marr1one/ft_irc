/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 14:28:04 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/21 13:52:00 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::start() {
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw std::runtime_error("socket failed\n");
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failed\n");
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl failed\n");
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_serverFd, (const sockaddr *)&addr, sizeof(addr)) < 0)
		throw std::runtime_error("Bind error\n");
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw std::runtime_error("Listen error\n");
	pollfd server_poll;
	server_poll.fd = _serverFd;
	server_poll.events = POLLIN;
	server_poll.revents = 0;
	_fds.push_back(server_poll);
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
	std::cout << "Server listening on port " << _port << std::endl;
	while (_running) {
		if (poll(&_fds[0], _fds.size(), -1) < 0) {
			if (!_running)
				break;
			throw std::runtime_error("poll error\n");
		}
		for (size_t i = 0; i < _fds.size(); i++) {
			const int currentFd = _fds[i].fd;
			const short revents = _fds[i].revents;

			if (revents & POLLIN) {
				if (currentFd == _serverFd)
					acceptClient();
				else
					receiveMessage(i);
			}
			if (i < _fds.size() && i > 0 && _fds[i].fd == currentFd &&
				(revents & POLLOUT))
				flushPendingWrites(currentFd);
		}
	}
	shutdown();
}
