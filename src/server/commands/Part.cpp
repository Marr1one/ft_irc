/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Part.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:22 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 21:02:59 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::handlePart(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 461 " + nick + " PART :Not enough parameters");
		return;
	}

	const std::string channelName = msg.params[0];

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

	const std::string partMsg =
		":" + nick + "!" + nick + "@ircserv PART " + channelName + "\r\n";
	sendChannelMsg(-1, channelName, partMsg);
	partChannel(fd, channelName);
}
