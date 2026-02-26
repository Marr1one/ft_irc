/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marwan <marwan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:17:58 by marwan            #+#    #+#             */
/*   Updated: 2026/02/24 23:38:43 by marwan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <map>
#include "Client.hpp"
#include <sys/socket.h>

class Channel
{
    private:
        std::string _name;
        std::map<int , Client*> _channelClients;
        
    public:
        Channel();
        Channel(std::string name);
        void addClient(Client *client);
        void removeClient(int fd);
        std::string get_name()const;
        void broadcast(int sender_fd, const std::string &msg);
};


#endif