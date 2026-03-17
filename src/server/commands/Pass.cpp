/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:25 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 14:50:45 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::handlePass(int fd, const Message &msg) {
	std::cout << "PASS command\n";

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv 461 * PASS :Not enough parameters");
		return;
	}

	Client &client = _clients[fd];
	const std::string &password = msg.params[0];

	if (client.get_registered()) {
		sendReply(fd, ":ircserv 462 " + client.get_nickname() +
						  " :You may not reregister");
		return;
	}

	if (password == _password) {
		client.set_pass(true);
		std::cout << "Password : OK\n";

		if (client.is_registerable() && !client.get_registered()) {
			client.set_registered(true);
			std::cout << "Client is now registered!\n";
			sendReply(fd, ":ircserv 001 " + client.get_nickname() +
							  " :Welcome to the IRC server " +
							  client.get_nickname() + "!");
		}
	} else {
		std::cout << "Password : Wrong\n";
		sendReply(fd, ":ircserv 464 * :Password incorrect");
	}
}
