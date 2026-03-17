/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:11 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 21:03:27 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::handleInvite(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.size() < 2) {
		sendReply(fd,
				  ":ircserv 461 " + nick + " INVITE :Not enough parameters");
		return;
	}

	const std::string target = msg.params[0];
	const std::string channelName = msg.params[1];

	if (_channels.find(channelName) == _channels.end()) {
		sendReply(fd, ":ircserv 403 " + nick + " " + channelName +
						  " :No such channel");
		return;
	}
	if (!_channels[channelName].hasClient(fd)) {
		sendReply(fd, ":ircserv 442 " + nick + " " + channelName +
						  " :You're not on that channel");
		return;
	}
	if (!_channels[channelName].isOperator(fd)) {
		sendReply(fd, ":ircserv 482 " + nick + " " + channelName +
						  " :You're not channel operator");
		return;
	}

	const int targetFd = findClientByNick(target);
	if (targetFd == -1) {
		sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
		return;
	}
	if (_channels[channelName].hasClient(targetFd)) {
		sendReply(fd, ":ircserv 443 " + nick + " " + target + " " +
						  channelName + " :is already on channel");
		return;
	}

	_channels[channelName].addInvited(targetFd);
	const std::string inviteMsg = ":" + nick + "!" + nick + "@ircserv INVITE " +
								  target + " :" + channelName;
	sendReply(targetFd, inviteMsg);
	sendReply(fd, ":ircserv 341 " + nick + " " + target + " " + channelName);
}
