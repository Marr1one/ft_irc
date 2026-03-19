/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:28 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handlePrivmsg(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 411 " + nick + " :No recipient given");
		return;
	}

	const std::string target = msg.params[0];
	if (!msg.hasTrailing || msg.trailing.empty()) {
		sendReply(fd, ":ircserv 412 " + nick + " :No text to send");
		return;
	}

	const std::string fullMsg = ":" + nick + "!" + nick + "@ircserv PRIVMSG "
		+ target + " :" + msg.trailing + "\r\n";

	if (target[0] == '#') {
		if (_channels.find(target) == _channels.end()) {
			sendReply(fd, ":ircserv 403 " + nick + " " + target + " :No such channel");
			return;
		}
		if (!_channels[target].hasClient(fd)) {
			sendReply(fd, ":ircserv 442 " + nick + " " + target + " :You're not on that channel");
			return;
		}
		sendChannelMsg(fd, target, fullMsg);
		return;
	}

	if (findClientByNick(target) == -1) {
		sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
		return;
	}
	sendUserMsg(fd, target, fullMsg);
}
