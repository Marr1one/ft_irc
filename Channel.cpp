/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:22:30 by marwan            #+#    #+#             */
/*   Updated: 2026/03/10 18:49:22 by braugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel():_name("Default channel"){};

Channel::Channel(const std::string &name)
    : _name(name), _topic(""),
      _invite_only(false),
      _topic_restricted(false),
      _key(""),
      _user_limit(-1)
{}
// Channel::Channel(std::string name):_name(name){}

void Channel::addClient(Client *client)
{
    _channelClients[client->get_fd()] = client;
}

void Channel::removeClient(int fd)
{
    _channelClients.erase(fd);
}
bool Channel::isInviteOnly() const       { return _invite_only; }
bool Channel::isTopicRestricted() const  { return _topic_restricted; }
std::string Channel::getKey() const      { return _key; }
int  Channel::getUserLimit() const       { return _user_limit; }

void Channel::setInviteOnly(bool val)           { _invite_only = val; }
void Channel::setTopicRestricted(bool val)      { _topic_restricted = val; }
void Channel::setKey(const std::string &key)    { _key = key; }
void Channel::setUserLimit(int limit)           { _user_limit = limit; }

int Channel::getClientCount() const
{
    return (int)_channelClients.size();
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

std::string Channel::getTopic() const { return this->_topic; }

void Channel::setTopic(const std::string &topic)
{
    this->_topic = topic;
}

std::string Channel::getClientList() const
{
    std::string list;
    for (std::map<int, Client*>::const_iterator it = _channelClients.begin();
         it != _channelClients.end(); it++)
    {
        if (!list.empty())
            list += " ";
        if (isOperator(it->first))
            list += "@";
        list += it->second->get_nickname();
    }
    return list;
}

void Channel::addOperator(int fd)
{
    for (std::vector<int>::iterator it = _operators.begin();
         it != _operators.end(); it++)
    {
        if (*it == fd) return;
    }
    _operators.push_back(fd);
}

void Channel::removeOperator(int fd)
{
    for (std::vector<int>::iterator it = _operators.begin();
         it != _operators.end(); it++)
    {
        if (*it == fd)
        {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::isOperator(int fd) const
{
    for (std::vector<int>::const_iterator it = _operators.begin();
         it != _operators.end(); it++)
    {
        if (*it == fd) return true;
    }
    return false;
}

bool Channel::hasClient(int fd) const
{
    return (_channelClients.find(fd) != _channelClients.end());
}

void Channel::addInvited(int fd)
{
    for (std::vector<int>::iterator it = _invited.begin();
         it != _invited.end(); it++)
    {
        if (*it == fd) return;
    }
    _invited.push_back(fd);
}

bool Channel::isInvited(int fd) const
{
    for (std::vector<int>::const_iterator it = _invited.begin();
         it != _invited.end(); it++)
    {
        if (*it == fd) return true;
    }
    return false;
}

void Channel::removeInvited(int fd)
{
    for (std::vector<int>::iterator it = _invited.begin();
         it != _invited.end(); it++)
    {
        if (*it == fd)
        {
            _invited.erase(it);
            return;
        }
    }
}