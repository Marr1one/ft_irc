/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 19:04:32 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 19:09:22 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleQuit(int fd, const Message &msg) {
	const std::string nick = _clients[fd].get_nickname();
	const std::string reason = msg.hasTrailing && !msg.trailing.empty()
		? msg.trailing : "Client Quit";
	const std::string quitMsg = ":" + nick + "!" + nick + "@ircserv QUIT :" + reason + "\r\n";

	for (ChannelIt it = _channels.begin(); it != _channels.end(); ++it)
		it->second.broadcast(fd, quitMsg);

	sendReply(fd, "ERROR :Closing connection");
	close(fd);
	removeClient(fd);
	std::cout << "Client " << nick << " disconnected\n";
}
