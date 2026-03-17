/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 16:16:44 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 16:20:58 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Message.hpp"

void Server::handlePing(int fd, const Message &msg)
{
	std::string token;

	if (msg.hasTrailing)
		token = msg.trailing;
	else if (!msg.params.empty())
		token = msg.params[0];
	else
		token = "ircserv";

	sendReply(fd, "PONG :" + token);
}