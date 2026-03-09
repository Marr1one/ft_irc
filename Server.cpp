/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:20:44 by marwan            #+#    #+#             */
/*   Updated: 2026/03/09 13:08:53 by braugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(char *port, char* password) : _port(atoi(port)),  _password(password)
{}

Server::~Server(){}

void Server::send_user_msg(int fd, std::string target,std::string msg)
{
    (void)fd;
    for(std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        if (it->second.get_nickname()==target)
        {
            send(it->second.get_fd(), msg.c_str(), msg.size(), 0);
            return ;
        }
    }
    std::cout << "User not found!\n";
}

void Server::removeClientServ(int fd)
{
    removeClientPoll(fd);
    _clients.erase(fd);
    for(std::map<std::string, Channel>::iterator it = _channels.begin();it != _channels.end();it++)
        it->second.removeClient(fd);
}

void Server::removeClientPoll(int fd)
{
    for(std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();it++)
        if (it->fd == fd) _fds.erase(it);
}

bool Server::checkNickname(const std::string &nickname) //je crois quen fait un vector de clients suffit car on peut avoir le fd dun client ! 
{
    for(std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end();it++)
    {
        if(it->second.get_nickname() == nickname)
            return (false);
    }
    return (true);
}

bool Server::checkUsername(const std::string &username)
{
    std::cout << "Checking username: '" << username << "'\n";  // ← ajoute ça
    for (std::map<int, Client>::iterator it = _clients.begin();
         it != _clients.end(); it++)
    {
        std::cout << "Comparing with: '" << it->second.get_username() << "'\n";  // ← et ça
        if (it->second.get_username() == username)
            return false;
    }
    return true;
}

void Server::part_channel(int fd,  std::string channelName)
{
    if (_channels.find(channelName) == _channels.end())
    {
        _channels[channelName] = Channel(channelName);
        std::cout << "Channel" << channelName << " not found\n";
    }
    else 
    {
        _channels[channelName].removeClient(fd);
        std::cout << "Client " << fd << " left channel : " << channelName << std::endl;
    }
}

void Server::send_channel_msg(int fd, std::string channelName, std::string msg)
{
    if (_channels.find(channelName) == _channels.end())
        std::cout << "Channel name not found!\n";
    else
        _channels[channelName].broadcast(fd, msg);
}

void Server::join_channel(int fd, std::string name)
{
    if (_channels.find(name) == _channels.end())
        _channels[name] = Channel(name);
    _channels[name].addClient(&_clients[fd]);
    std::cout << "Client " << fd << " joined channel : " << name << std::endl;
}


void Server::parseCommand(int fd, std::string str)
{
    std::stringstream ss(str);
    std::string token;
    ss >> token;
    if (!_clients[fd].is_registerable())
    {
        if (token != "PASS" && token != "USER" &&  token != "NICK" &&  token != "QUIT")
        {
            std::cout << "Client " << _clients[fd].get_fd() << "can't use " << token << ", he need to register first!\n";
            return;
        }
    }
    if (token == "PASS")
    {
        std::cout << "PASS command\n";
        std::string password;
        ss >> password;
        if (_clients[fd].get_registered())
        {
            sendReply(fd, ":ircserv 462 " + _clients[fd].get_nickname() + " :You may not reregister");
            return;
        }
        if (password == _password)
        {
            _clients[fd].set_pass(true);
            std::cout << "Password : OK\n";
            if (_clients[fd].is_registerable() && !_clients[fd].get_registered())
            {
                _clients[fd].set_registered(true);
                std::cout << "Client is now registered!\n";
            }
        }
        else {
            std::cout <<"Password : Wrong\n";
            sendReply(fd, ":ircserv 464 * :Password incorret");
        }
    }
    else if (token == "NICK")
    {
        std::cout << "NICK command\n";
        std::string nickname;
        ss >> nickname;
        if (nickname.empty())
        {
            sendReply(fd, ":ircserv 431 * :No nickname given");
            return;
        }
        if (!checkNickname(nickname))
        {
            sendReply(fd, ":ircserv 433 * " + nickname + " :Nickname is already in use"); 
            return;
        }
        _clients[fd].set_nickname(nickname);
        _clients[fd].set_nickOK(true);
        std::cout << "Nickname set : " << nickname << std::endl;
        if (_clients[fd].is_registerable() && !_clients[fd].get_registered())
        {
            _clients[fd].set_registered(true);
            sendReply(fd, ":ircserv 001 " + nickname + " :Welcome to the IRC server " + nickname + "!");
        }
    }
    else if (token == "USER")
    {
        std::cout << "USER command\n";
        std::string username;
        ss >> username;
        if (username.empty())
        {
            sendReply(fd, ":ircserv 461 * USER :Not enough parameters");            
            return;
        }
        if (!checkUsername(username))
        {
            sendReply(fd, ":ircserv 462 * :Username already in use");
            return;
        }
        _clients[fd].set_user(username);
        _clients[fd].set_userOK(true);
        if (_clients[fd].is_registerable() && !_clients[fd].get_registered())
        {
            _clients[fd].set_registered(true);
            std::cout << "Client is now registered!\n";
            std::string nick = _clients[fd].get_nickname();
            sendReply(fd, ":ircserv 001 " + nick + " :Welcome to the IRC server " + nick + "!");
        }
    }
    else if (token=="JOIN")
    {
        std::string channelName;
        ss >> channelName;
        join_channel(fd, channelName);
    }
    else if (token == "PRIVMSG")
    {
        std::string target;
        ss >> target;
        if (target[0]=='#')
        {
            std::cout <<"direction channel";
            std::string msg;
            getline(ss, msg);
            send_channel_msg(fd, target, msg);
            std::cout << "PRIVMSG command\n";

        }
        else
        {
            std::cout<<"direction un user je crois\n";
            std::string msg;
            getline(ss, msg);
            send_user_msg(fd, target, msg);
            std::cout << "PRIVMSG command\n";
        }
        
    }
    else if (token == "PART")
    {
        std::string channelName;
        ss >> channelName;
        part_channel(fd, channelName);
    }
    else if (token == "QUIT")
    {
        removeClientServ(fd);
        std::cout << "Client " << fd << " have been remove from all channels and his fd has been erase from the server.\n";
    }
    else if (token == "KICK");
    else if (token == "INVITE");
    else if (token == "TOPIC");
    else if (token == "MODE");
    else std::cout << "Unknow command\n";
}

void Server::acceptClient()
{
    int client_fd = accept(_server_fd, NULL, NULL);
    if (client_fd < 0) return;//ptet ajouter un message derreur
    pollfd newClient;
    newClient.fd = client_fd;
    newClient.events = POLLIN;
    newClient.revents = 0;
    _fds.push_back(newClient); 
    _clients[client_fd] = Client(client_fd);
    std::cout << "New client connected! fd : " << client_fd << std::endl;
}

void Server::receiveMessage(int i)
{
    char buffer[1024];//jimagine qui faut checker si le message est pas trop long! faisable facile avec substr
    int fd = _fds[i].fd;
    int bytes = recv(_fds[i].fd,buffer,sizeof(buffer) -1, 0);
    if (bytes <= 0)//-1 cest erreur nan ? 
    {
        std::cout << "Client " << fd << " disconnected\n";
        close(_fds[i].fd);
        removeClientServ(fd);
        return;
    }
    buffer[bytes]='\0';
    _clients[fd].appendBuffer(buffer);
    while (_clients[fd].hasLine())
    {
        std::string line = _clients[fd].extractLine();
        if (!line.empty())
            parseCommand(fd, line);
    }
}

void Server::sendReply(int fd, const std::string &reply)
{
    std::string msg = reply + "\r\n";
    send(fd, msg.c_str(), msg.size(), 0);
}

void Server::start()
{
    _server_fd = socket(AF_INET, SOCK_STREAM,0);
    if (_server_fd<0)throw std::runtime_error("socket failed\n");
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_server_fd,(const sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind error\n");
    if(listen(_server_fd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen error\n");
    pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    server_poll.revents =0;
    _fds.push_back(server_poll);
    std::cout << "Server listening on port " << _port << std::endl;
    while(1)
    {
        if(poll(&_fds[0], _fds.size(), -1) < 0)
            throw std::runtime_error("poll error\n");
        for(size_t i = 0; i< _fds.size();i++)
        {
            if(_fds[i].revents & POLLIN)
            {
                if(_fds[i].fd == _server_fd)
                    acceptClient();
                else receiveMessage(i);
            }
        }
    }
}