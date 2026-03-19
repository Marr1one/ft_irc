/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cap.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:17:39 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 16:21:17 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handleCap(int fd, const Message &msg)
{
	if (msg.params.empty())
		return;

	const std::string &subcommand = msg.params[0];

	if (subcommand == "LS")
		sendReply(fd, ":ircserv CAP * LS :");
	else if (subcommand == "REQ")
	{
		if (msg.hasTrailing && !msg.trailing.empty())
			sendReply(fd, ":ircserv CAP * NAK :" + msg.trailing);
		else
			sendReply(fd, ":ircserv CAP * NAK :");
	}
	else if (subcommand == "END")
		return;
}
