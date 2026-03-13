/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:16:22 by marwan            #+#    #+#             */
/*   Updated: 2026/03/10 10:53:48 by braugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"


int main(int argc, char **argv)
{
    if(argc!=3)return (std::cerr<<"Usage: ./irc port password\n",1);
    try
    {
        Server server(argv[1], argv[2]);
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}