/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:00:51 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/19 00:30:27 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

void Server::initCommandHandlers() {
	_handlers["PASS"] = &Server::handlePass;
	_handlers["NICK"] = &Server::handleNick;
	_handlers["USER"] = &Server::handleUser;
	_handlers["JOIN"] = &Server::handleJoin;
	_handlers["PRIVMSG"] = &Server::handlePrivmsg;
	_handlers["BLACKJACK"] = &Server::handleBlackjack;
	_handlers["PART"] = &Server::handlePart;
	_handlers["QUIT"] = &Server::handleQuit;
	_handlers["KICK"] = &Server::handleKick;
	_handlers["INVITE"] = &Server::handleInvite;
	_handlers["TOPIC"] = &Server::handleTopic;
	_handlers["MODE"] = &Server::handleMode;
	_handlers["PING"] = &Server::handlePing;
	_handlers["CAP"] = &Server::handleCap;
}

bool Server::isAllowedBeforeRegistration(const std::string &command) const {
	return (command == "PASS" || command == "NICK" || command == "USER" ||
			command == "QUIT" || command == "CAP" || command == "PING");
}

void Server::handleMessage(int fd, const Message &msg) {
	if (msg.command.empty())
		return;

	ClientIt clientIt = _clients.find(fd);
	if (clientIt == _clients.end())
		return;

	if (!clientIt->second.get_registered() &&
		!isAllowedBeforeRegistration(msg.command)) {
		std::cout << "Client " << fd << " can't use " << msg.command
				  << ", needs to register first!\n";
		return;
	}

	HandlerIt it = _handlers.find(msg.command);
	if (it == _handlers.end()) {
		// TO-DO: send 421 for unkown command for irssi
		std::cout << "Unknown command: " << msg.command << std::endl;
		return;
	}

	(this->*(it->second))(fd, msg);
}
