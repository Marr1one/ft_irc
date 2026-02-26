/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marwan <marwan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:22:30 by marwan            #+#    #+#             */
/*   Updated: 2026/02/24 23:38:41 by marwan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel():_name("Default channel"){};

Channel::Channel(std::string name):_name(name){}

void Channel::addClient(Client *client)
{
    _channelClients[client->get_fd()] = client;
}

void Channel::removeClient(int fd)
{
    _channelClients.erase(fd);
}

std::string Channel::get_name()const{return this->_name;}

void Channel::broadcast(int sender_fd, const std::string &msg)
{
    for(std::map<int, Client*>::iterator it = _channelClients.begin(); it != _channelClients.end();it++)
    {
        if (it->first != sender_fd)
            send(it->first, msg.c_str(), msg.size(),0);
    }
}