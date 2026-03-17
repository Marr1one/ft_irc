/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:38 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 14:59:59 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::handleUser(int fd, const Message &msg) {
	std::cout << "USER command\n";

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 461 * USER :Not enough parameters");
		return;
	}

	Client &client = _clients[fd];
	const std::string &username = msg.params[0];

	if (!checkUsername(username)) {
		sendReply(fd, ":ircserv 462 * :Username already in use");
		return;
	}

	client.set_user(username);
	client.set_userOK(true);

	if (client.is_registerable() && !client.get_registered()) {
		client.set_registered(true);
		std::cout << "Client is now registered!\n";

		const std::string &nick = client.get_nickname();
		sendReply(fd, ":ircserv 001 " + nick + " :Welcome to the IRC server " +
						  nick + "!");
	}
}