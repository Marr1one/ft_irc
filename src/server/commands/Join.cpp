/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:13 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 21:03:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::handleJoin(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 461 " + nick + " JOIN :Not enough parameters");
		return;
	}

	const std::string channelName = msg.params[0];
	const std::string key = (msg.params.size() > 1) ? msg.params[1] : "";

	if (channelName.empty() || channelName[0] != '#') {
		sendReply(fd, ":ircserv 403 " + nick + " " + channelName +
						  " :No such channel");
		return;
	}

	if (_channels.find(channelName) != _channels.end()) {
		if (!_channels[channelName].getKey().empty() &&
			key != _channels[channelName].getKey()) {
			sendReply(fd, ":ircserv 475 " + nick + " " + channelName +
							  " :Cannot join channel (+k)");
			return;
		}
		if (_channels[channelName].isInviteOnly() &&
			!_channels[channelName].isInvited(fd)) {
			sendReply(fd, ":ircserv 473 " + nick + " " + channelName +
							  " :Cannot join channel (+i)");
			return;
		}
		if (_channels[channelName].getUserLimit() != -1 &&
			_channels[channelName].getClientCount() >=
				_channels[channelName].getUserLimit()) {
			sendReply(fd, ":ircserv 471 " + nick + " " + channelName +
							  " :Cannot join channel (+l)");
			return;
		}
	}

	joinChannel(fd, channelName);
	std::string joinMsg =
		":" + nick + "!" + nick + "@ircserv JOIN " + channelName + "\r\n";
	sendChannelMsg(-1, channelName, joinMsg);
	std::string topic = _channels[channelName].getTopic();
	if (topic.empty())
		sendReply(fd, ":ircserv 331 " + nick + " " + channelName +
						  " :No topic is set");
	else
		sendReply(fd,
				  ":ircserv 332 " + nick + " " + channelName + " :" + topic);

	sendReply(fd, ":ircserv 353 " + nick + " = " + channelName + " :" +
					  _channels[channelName].getClientList());
	sendReply(fd, ":ircserv 366 " + nick + " " + channelName +
					  " :End of /NAMES list");
	if (_channels[channelName].isInvited(fd))
		_channels[channelName].removeInvited(fd);
}
