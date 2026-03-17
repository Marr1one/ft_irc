/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:16 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleKick(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.size() < 2) {
		sendReply(fd, ":ircserv 461 " + nick + " KICK :Not enough parameters");
		return;
	}

	const std::string channelName = msg.params[0];
	const std::string target = msg.params[1];

	if (_channels.find(channelName) == _channels.end()) {
		sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
		return;
	}
	if (!_channels[channelName].isOperator(fd)) {
		sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
		return;
	}

	const int targetFd = findClientByNick(target);
	if (targetFd == -1) {
		sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
		return;
	}
	if (!_channels[channelName].hasClient(targetFd)) {
		sendReply(fd, ":ircserv 441 " + nick + " " + target + " " + channelName + " :They aren't on that channel");
		return;
	}

	const std::string kickMsg = ":" + nick + "!" + nick + "@ircserv KICK "
		+ channelName + " " + target + "\r\n";
	_channels[channelName].broadcast(-1, kickMsg);
	_channels[channelName].removeClient(targetFd);
	_channels[channelName].removeOperator(targetFd);
}
