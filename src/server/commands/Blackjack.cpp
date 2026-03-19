/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Blackjack.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 20:39:15 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/19 13:19:45 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include "Server.hpp"

#include <cctype>
#include <sstream>

static std::string toUpperCopy(const std::string &value) {
	std::string upper = value;

	for (std::string::size_type i = 0; i < upper.size(); ++i)
		upper[i] = static_cast<char>(std::toupper(upper[i]));
	return upper;
}

static std::string intToString(int value) {
	std::ostringstream oss;

	oss << value;
	return oss.str();
}

void Server::handleBlackjack(int fd, const Message &msg) {
	Client &client = _clients[fd];
	const std::string nick = client.get_nickname();
	BlackjackGame &game = _blackjackGames[fd];

	if (msg.params.empty()) {
		sendReply(fd, ":ircserv NOTICE " + nick +
						" :Usage: BLACKJACK START|HIT|STAND|STATUS");
		return;
	}

	const std::string action = toUpperCopy(msg.params[0]);

	if (action == "START") {
		game.start();
		sendReply(fd, ":ircserv NOTICE " + nick + " :Blackjack started.");
		sendReply(fd, ":ircserv NOTICE " + nick + " :Your hand: " +
						game.getPlayerHandString() + " (" +
						intToString(game.getPlayerValue()) + ")");
		sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer shows: " +
						game.getDealerHandString(game.isFinished()));
		if (game.isFinished()) {
			sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer hand: " +
							game.getDealerHandString(true) + " (" +
							intToString(game.getDealerValue()) + ")");
			sendReply(fd, ":ircserv NOTICE " + nick + " :" + game.getResult());
		}
		return;
	}

	if (!game.isActive()) {
		sendReply(fd, ":ircserv NOTICE " + nick +
						" :No active blackjack game. Use BLACKJACK START.");
		return;
	}

	if (action == "HIT") {
		if (game.isFinished()) {
			sendReply(fd, ":ircserv NOTICE " + nick +
							" :Game is already finished. Use BLACKJACK START.");
			return;
		}
		game.playerHit();
		sendReply(fd, ":ircserv NOTICE " + nick + " :Your hand: " +
						game.getPlayerHandString() + " (" +
						intToString(game.getPlayerValue()) + ")");
		if (game.isFinished())
		{
			sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer hand: " +
							game.getDealerHandString(true) + " (" +
							intToString(game.getDealerValue()) + ")");
			sendReply(fd, ":ircserv NOTICE " + nick + " :" + game.getResult());
		}
		else
			sendReply(fd, ":ircserv NOTICE " + nick +
							" :Dealer shows: " + game.getDealerHandString(false));
		return;
	}

	if (action == "STAND") {
		if (game.isFinished()) {
			sendReply(fd, ":ircserv NOTICE " + nick +
							" :Game is already finished. Use BLACKJACK START.");
			return;
		}
		game.playerStand();
		sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer hand: " +
						game.getDealerHandString(true) + " (" +
						intToString(game.getDealerValue()) + ")");
		sendReply(fd, ":ircserv NOTICE " + nick + " :" + game.getResult());
		return;
	}

	if (action == "STATUS") {
		sendReply(fd, ":ircserv NOTICE " + nick + " :Your hand: " +
						game.getPlayerHandString() + " (" +
						intToString(game.getPlayerValue()) + ")");
		sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer shows: " +
						game.getDealerHandString(game.isFinished()));
		if (game.isFinished())
		{
			sendReply(fd, ":ircserv NOTICE " + nick + " :Dealer hand: " +
							game.getDealerHandString(true) + " (" +
							intToString(game.getDealerValue()) + ")");
			sendReply(fd, ":ircserv NOTICE " + nick + " :" + game.getResult());
		}
		return;
	}

	sendReply(fd, ":ircserv NOTICE " + nick +
					" :Unknown blackjack action. Use START, HIT, STAND, or STATUS.");
}
