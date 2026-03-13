/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:17:48 by marwan            #+#    #+#             */
/*   Updated: 2026/03/10 11:55:33 by braugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <vector>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <map>
#include "Client.hpp"
#include <sstream>
#include "Channel.hpp"

#ifndef SERVER_HPP
#define SERVER_HPP

class Server
{
    private:
    int _port;
    std::string _password;
    int _server_fd;
    std::vector<pollfd>_fds;
    std::map<int,Client> _clients;
    std::map<std::string, Channel> _channels;
    public:
    Server(char *port, char* password);
    ~Server();
    void start();
        void acceptClient();
        void receiveMessage(int i);
        void parseCommand(int fd, std::string str);
        void join_channel(int fd, std::string name);
        void send_channel_msg(int fd, std::string channelName, std::string msg);
        void removeClientServ(int fd);
        void removeClientPoll(int fd);
        void part_channel(int fd, std::string channelName);
        bool checkNickname(const std::string &name);
        bool checkUsername(const std::string &username); //b
        void send_user_msg(int fd, std::string target,std::string msg);
        void sendReply(int fd, const std::string &reply); //b
        int findClientByNick(const std::string &nick); //b
        static void signalHandler(int signal);
        static bool _running;
};

#endif