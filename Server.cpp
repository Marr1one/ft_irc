/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:20:44 by marwan            #+#    #+#             */
/*   Updated: 2026/03/10 19:37:01 by braugust         ###   ########.fr       */
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
    for(std::map<std::string, Channel>::iterator it = _channels.begin();it != _channels.end();it++)
        it->second.removeClient(fd);
    _clients.erase(fd);
}

bool Server::_running = true;

void Server::signalHandler(int signal)
{
    (void)signal;
    std::cout << "\nServer shutting down...\n";
    _running = false;
}

void Server::removeClientPoll(int fd)
{
    for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++)
    {
        if (it->fd == fd)
        {
            _fds.erase(it);
            return;
        }
    }
}

int Server::findClientByNick(const std::string &nick)
{
    for (std::map<int, Client>::iterator it = _clients.begin();
         it != _clients.end(); it++)
    {
        if (it->second.get_nickname() == nick)
            return it->first;
    }
    return -1;
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
    std::cout << "Checking username: '" << username << "'\n";
    for (std::map<int, Client>::iterator it = _clients.begin();
         it != _clients.end(); it++)
    {
        std::cout << "Comparing with: '" << it->second.get_username() << "'\n";
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
    bool isNew = (_channels.find(name) == _channels.end());

    if (isNew)
        _channels[name] = Channel(name);

    Client *client = &_clients[fd];
    _channels[name].addClient(client);
    if (isNew)
        _channels[name].addOperator(fd);
    std::cout << "Client " << fd << " joined channel : " << name;
    if (isNew) std::cout << " (created, operator)";
    std::cout << std::endl;
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
    else if (token == "JOIN")
    {
        std::string channelName, key;
        ss >> channelName >> key;
        std::string nick = _clients[fd].get_nickname();
        if (channelName.empty())
        {
            sendReply(fd, ":ircserv 461 " + nick + " JOIN :Not enough parameters");
            return;
        }
        if (channelName[0] != '#')
        {
            sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
            return;
        }
        if (_channels.find(channelName) != _channels.end())
        {
            if (!_channels[channelName].getKey().empty() && key != _channels[channelName].getKey())
            {
                sendReply(fd, ":ircserv 475 " + nick + " " + channelName + " :Cannot join channel (+k)");
                return;
            }
            if (_channels[channelName].isInviteOnly() && !_channels[channelName].isInvited(fd))
            {
                sendReply(fd, ":ircserv 473 " + nick + " " + channelName + " :Cannot join channel (+i)");
                return;
            }
            if (_channels[channelName].getUserLimit() != -1
                && _channels[channelName].getClientCount() >= _channels[channelName].getUserLimit())
            {
                sendReply(fd, ":ircserv 471 " + nick + " " + channelName + " :Cannot join channel (+l)");
                return;
            }
        }
        join_channel(fd, channelName);
        std::string joinMsg = ":" + nick + "!" + nick + "@ircserv JOIN " + channelName + "\r\n";
        _channels[channelName].broadcast(-1, joinMsg);
        std::string topic = _channels[channelName].getTopic();
        if (topic.empty())
            sendReply(fd, ":ircserv 331 " + nick + " " + channelName + " :No topic is set");
        else
            sendReply(fd, ":ircserv 332 " + nick + " " + channelName + " :" + topic);

        sendReply(fd, ":ircserv 353 " + nick + " = " + channelName + " :" + _channels[channelName].getClientList());
        sendReply(fd, ":ircserv 366 " + nick + " " + channelName + " :End of /NAMES list");
        if (_channels[channelName].isInvited(fd))
            _channels[channelName].removeInvited(fd);
    }
    else if (token == "PRIVMSG")
    {
        std::string target;
        ss >> target;

        if (target.empty())
        {
            sendReply(fd, ":ircserv 411 " + _clients[fd].get_nickname() + " :No recipient given");
            return;
        }
        std::string msg;
        getline(ss, msg);
        if (msg.empty())
        {
            sendReply(fd, ":ircserv 412 " + _clients[fd].get_nickname() + " :No text to send");
            return;
        }
        std::string nick = _clients[fd].get_nickname();
        std::string fullMsg = ":" + nick + " @ircserv PRIVMSG " + target + msg + "\r\n";    
        if (target[0] == '#')
        {
            if (_channels.find(target) == _channels.end())
            {
                sendReply(fd, ":ircserv 403 " + nick + " " + target + " :No such channel");
                return;
            }
            send_channel_msg(fd, target, fullMsg);
        }
        else
        {
            if (findClientByNick(target) == -1)
            {
                sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
                return;
            }
            send_user_msg(fd, target, fullMsg);
        }
    }
    else if (token == "PART")
    {
        std::string channelName;
        ss >> channelName;
        if (channelName.empty())
        {
            sendReply(fd, ":ircserv 461 " + _clients[fd].get_nickname() + " PART :Not enough parameters");
            return;
        }
        if (_channels.find(channelName) == _channels.end())
        {
            sendReply(fd, ":ircserv 403 " + _clients[fd].get_nickname() + " " + channelName + " :No such channel");
            return;
        }
        std::string nick = _clients[fd].get_nickname();
        std::string partMsg = ":" + nick + "@ircserv PART " + channelName + "\r\n";
        _channels[channelName].broadcast(-1, partMsg);
        _channels[channelName].removeClient(fd);
    }
    else if (token == "QUIT")
    {
        std::string nick = _clients[fd].get_nickname();

        std::string quitMsg = ":" + nick + " @ircserv QUIT" + "\r\n";
        for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
        {
            it->second.broadcast(fd, quitMsg);
        }
        sendReply(fd, "ERROR :Closing connection");
        close(fd);
        removeClientServ(fd);
        std::cout << "Client " << nick << " disconnected\n";
    }
    else if (token == "KICK")
    {
        std::string channelName, target;
        ss >> channelName >> target;
        std::string nick = _clients[fd].get_nickname();
        if (_channels.find(channelName) == _channels.end())
        {
            sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
            return;
        }
        if (!_channels[channelName].isOperator(fd))
        {
            sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
            return;
        }
        int targetFd = findClientByNick(target);
        if (targetFd == -1)
        {
            sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
            return;
        }
        if (!_channels[channelName].hasClient(targetFd))
        {
            sendReply(fd, ":ircserv 441 " + nick + " " + target + " " + channelName + " :They aren't on that channel");
            return;
        }
        std::string kickMsg = ":" + nick + "@ircserv KICK "+ channelName + " " + target + "\r\n";
        _channels[channelName].broadcast(-1, kickMsg);
        _channels[channelName].removeClient(targetFd);
        _channels[channelName].removeOperator(targetFd);
    }
    else if (token == "INVITE")
    {
        std::string target, channelName;
        ss >> target >> channelName;
        std::string nick = _clients[fd].get_nickname();
        if (_channels.find(channelName) == _channels.end())
        {
            sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
            return;
        }
        if (!_channels[channelName].isOperator(fd))
        {
            sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
            return;
        }
        int targetFd = findClientByNick(target);
        if (targetFd == -1)
        {
            sendReply(fd, ":ircserv 401 " + nick + " " + target + " :No such nick");
            return;
        }
        if (_channels[channelName].hasClient(targetFd))
        {
            sendReply(fd, ":ircserv 443 " + nick + " " + target + " " + channelName + " :is already on channel");
            return;
        }
        _channels[channelName].addInvited(targetFd);
        std::string inviteMsg = ":" + nick + "@ircserv INVITE " + target + " :" + channelName + "\r\n";
        // sendReply(targetFd, inviteMsg);
        sendReply(fd, ":ircserv 341 " + nick + " " + target + " " + channelName);
    }
    else if (token == "TOPIC")
    {
        std::string channelName, newTopic;
        ss >> channelName;
        std::string nick = _clients[fd].get_nickname();
        if (_channels.find(channelName) == _channels.end())
        {
            sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
            return;
        }
        if (!_channels[channelName].hasClient(fd))
        {
            sendReply(fd, ":ircserv 442 " + nick + " " + channelName + " :You're not on that channel");
            return;
        }
        getline(ss, newTopic);
        if (newTopic.empty())
        {
            std::string topic = _channels[channelName].getTopic();
            if (topic.empty())
                sendReply(fd, ":ircserv 331 " + nick + " " + channelName + " :No topic is set");
            else
                sendReply(fd, ":ircserv 332 " + nick + " " + channelName + " :" + topic);
            return;
        }
        if (_channels[channelName].isTopicRestricted() && !_channels[channelName].isOperator(fd))
        {
            sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
            return;
        }
        if (newTopic[0] == ' ') 
            newTopic = newTopic.substr(1);
        if (!newTopic.empty() && newTopic[0] == ':')
            newTopic = newTopic.substr(1);
        _channels[channelName].setTopic(newTopic);
        std::string topicMsg = ":" + nick + "@ircserv TOPIC "
                            + channelName + " :" + newTopic + "\r\n";
        _channels[channelName].broadcast(-1, topicMsg);
    }
    else if (token == "MODE")
    {
        std::string channelName, modeStr, param;
        ss >> channelName >> modeStr >> param;
        std::string nick = _clients[fd].get_nickname();
        if (_channels.find(channelName) == _channels.end())
        {
            sendReply(fd, ":ircserv 403 " + nick + " " + channelName + " :No such channel");
            return;
        }
        if (!_channels[channelName].isOperator(fd))
        {
            sendReply(fd, ":ircserv 482 " + nick + " " + channelName + " :You're not channel operator");
            return;
        }
        if (modeStr.empty() || (modeStr[0] != '+' && modeStr[0] != '-'))
        {
            sendReply(fd, ":ircserv 472 " + nick + " :Unknown mode");
            return;
        }
        bool adding = (modeStr[0] == '+');
        char mode   = modeStr[1];
        if (mode == 'i')
        {
            _channels[channelName].setInviteOnly(adding);
            std::string msg = ":" + nick  + " @ircserv MODE "
                            + channelName + " " + modeStr + "\r\n";
            _channels[channelName].broadcast(-1, msg);
        }
        else if (mode == 't')
        {
            _channels[channelName].setTopicRestricted(adding);
            std::string msg = ":" + nick + " @ircserv MODE "
                            + channelName + " " + modeStr + "\r\n";
            _channels[channelName].broadcast(-1, msg);
        }
        else if (mode == 'k')
        {
            if (adding)
            {
                if (param.empty())
                {
                    sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
                    return;
                }
                _channels[channelName].setKey(param);
            }
            else
                _channels[channelName].setKey("");  // supprimer le password

            std::string msg = ":" + nick  + " @ircserv MODE "
                            + channelName + " " + modeStr + "\r\n";
            if (adding) msg += " " + param;
            _channels[channelName].broadcast(-1, msg);
        }
        else if (mode == 'o')
        {
            if (param.empty())
            {
                sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
                return;
            }
            int targetFd = findClientByNick(param);
            if (targetFd == -1)
            {
                sendReply(fd, ":ircserv 401 " + nick + " " + param + " :No such nick");
                return;
            }
            if (!_channels[channelName].hasClient(targetFd))
            {
                sendReply(fd, ":ircserv 441 " + nick + " " + param + " " + channelName + " :They aren't on that channel");
                return;
            }
            if (adding)
                _channels[channelName].addOperator(targetFd);
            else
                _channels[channelName].removeOperator(targetFd);

            std::string msg = ":" + nick + " @ircserv MODE "
                            + channelName + " " + modeStr + " " + param + "\r\n";
            _channels[channelName].broadcast(-1, msg);
        }
        else if (mode == 'l')
        {
            if (adding)
            {
                if (param.empty())
                {
                    sendReply(fd, ":ircserv 461 " + nick + " MODE :Not enough parameters");
                    return;
                }
                int limit = atoi(param.c_str());
                if (limit <= 0)
                {
                    sendReply(fd, ":ircserv 461 " + nick + " MODE :Invalid limit");
                    return;
                }
                _channels[channelName].setUserLimit(limit);
            }
            else
                _channels[channelName].setUserLimit(-1);

            std::string msg = ":" + nick + "@ircserv MODE "
                            + channelName + " " + modeStr + "\r\n";
            if (adding) msg += " " + param;
            _channels[channelName].broadcast(-1, msg);
        }
        else
        {
            sendReply(fd, ":ircserv 472 " + nick + " " + modeStr + " :Unknown mode char");
        }
    }
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
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) throw std::runtime_error("socket failed\n");
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_server_fd, (const sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Bind error\n");
    if (listen(_server_fd, SOMAXCONN) < 0)
        throw std::runtime_error("Listen error\n");
    pollfd server_poll;
    server_poll.fd = _server_fd;
    server_poll.events = POLLIN;
    server_poll.revents = 0;
    _fds.push_back(server_poll);
    signal(SIGINT, Server::signalHandler);
    signal(SIGQUIT, Server::signalHandler);
    std::cout << "Server listening on port " << _port << std::endl;
    while (_running)
    {
        if (poll(&_fds[0], _fds.size(), -1) < 0)  // ← un seul appel
        {
            if (!_running) break;  // signal reçu → sortie propre
            throw std::runtime_error("poll error\n");
        }
        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _server_fd)
                    acceptClient();
                else
                    receiveMessage(i);
            }
        }
    }
    close(_server_fd);
}