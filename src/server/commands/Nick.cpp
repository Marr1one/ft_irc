/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:20 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleNick(int fd, const Message &msg) {
	std::cout << "NICK command\n";

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 431 * :No nickname given");
		return;
	}

	Client &client = _clients[fd];
	const std::string &nickname = msg.params[0];

	if (!checkNickname(nickname)) {
		sendReply(fd, ":ircserv 433 * " + nickname + " :Nickname is already in use");
		return;
	}

	client.set_nickname(nickname);
	client.set_nickOK(true);
	std::cout << "Nickname set : " << nickname << std::endl;

	if (canFinishRegistration(client) && !client.get_registered()) {
		client.set_registered(true);
		sendReply(fd, ":ircserv 001 " + nickname + " :Welcome to the IRC server " + nickname + "!");
	}
}
