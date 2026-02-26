/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marwan <marwan@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:16:22 by marwan            #+#    #+#             */
/*   Updated: 2026/02/24 23:43:43 by marwan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Server.hpp"
#include "Client.hpp"


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