/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:35 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleTopic(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 461 " + nick + " TOPIC :Not enough parameters");
		return;
	}

	const std::string channelName = msg.params[0];
	if (_channels.find(channelName) == _channels.end()) {
		sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
		return;
	}
	if (!_channels[channelName].hasClient(fd)) {
		sendReply(fd, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
		return;
	}

	if (!msg.hasTrailing) {
		const std::string topic = _channels[channelName].getTopic();
		if (topic.empty())
			sendReply(fd, ":ircserv 331 " + nick + " " + channelName + " :No topic is set");
		else
			sendReply(fd, ":ircserv 332 " + nick + " " + channelName + " :" + topic);
		return;
	}

	if (_channels[channelName].isTopicRestricted() && !_channels[channelName].isOperator(fd)) {
		sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
		return;
	}

	_channels[channelName].setTopic(msg.trailing);
	const std::string topicMsg = ":" + nick + "!" + nick + "@ircserv TOPIC "
		+ channelName + " :" + msg.trailing + "\r\n";
	_channels[channelName].broadcast(-1, topicMsg);
}
