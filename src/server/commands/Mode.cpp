/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:18 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleMode(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();

	if (msg.params.size() < 2) {
		sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
		return;
	}

	const std::string channelName = msg.params[0];
	const std::string modeStr = msg.params[1];
	const std::string param = (msg.params.size() > 2) ? msg.params[2] : "";

	if (_channels.find(channelName) == _channels.end()) {
		sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
		return;
	}
	if (!_channels[channelName].isOperator(fd)) {
		sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
		return;
	}
	if (modeStr.size() < 2 || (modeStr[0] != '+' && modeStr[0] != '-')) {
		sendReply(fd, ":ircserv 472 " + nick + " :Unknown mode");
		return;
	}

	const bool adding = (modeStr[0] == '+');
	const char mode = modeStr[1];
	std::string modeMsg;

	if (mode == 'i') {
		_channels[channelName].setInviteOnly(adding);
		modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + "\r\n";
	} else if (mode == 't') {
		_channels[channelName].setTopicRestricted(adding);
		modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + "\r\n";
	} else if (mode == 'k') {
		if (adding) {
			if (param.empty()) {
				sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
				return;
			}
			_channels[channelName].setKey(param);
			modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + " " + param + "\r\n";
		} else {
			_channels[channelName].setKey("");
			modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + "\r\n";
		}
	} else if (mode == 'o') {
		if (param.empty()) {
			sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
			return;
		}
		const int targetFd = findClientByNick(param);
		if (targetFd == -1) {
			sendReply(fd, ":ircserv 401 " + nick + " " + param + " :No such nick");
			return;
		}
		if (!_channels[channelName].hasClient(targetFd)) {
			sendReply(fd, ":ircserv 441 " + nick + " " + param + " " + channelName + " :They aren't on that channel");
			return;
		}
		if (adding)
			_channels[channelName].addOperator(targetFd);
		else
			_channels[channelName].removeOperator(targetFd);
		modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + " " + param + "\r\n";
	} else if (mode == 'l') {
		if (adding) {
			if (param.empty()) {
				sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
				return;
			}
			const int limit = std::atoi(param.c_str());
			if (limit <= 0) {
				sendReply(fd, ":ircserv 461 " + nick + " MODE :Invalid limit");
				return;
			}
			_channels[channelName].setUserLimit(limit);
			modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + " " + param + "\r\n";
		} else {
			_channels[channelName].setUserLimit(-1);
			modeMsg = ":" + nick + "!" + nick + "@ircserv MODE " + channelName + " " + modeStr + "\r\n";
		}
	} else {
		sendReply(fd, ":ircserv 472 " + nick + " " + modeStr + " :Unknown mode char");
		return;
	}

	_channels[channelName].broadcast(-1, modeMsg);
}
