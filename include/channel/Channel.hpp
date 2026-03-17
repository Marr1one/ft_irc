/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 20:17:58 by marwan            #+#    #+#             */
/*   Updated: 2026/03/14 22:15:33 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <vector>

class Channel {
  private:
	std::string _name;
	std::string _topic;
	std::map<int, Client *> _channelClients;
	std::vector<int> _operators;
	std::vector<int> _invited;
	bool _invite_only;		// +i
	bool _topic_restricted; // +t
	std::string _key;		// +k
	int _user_limit;		// +l

  public:
	Channel();
	Channel(const std::string &name);
	void addClient(Client *client);
	void removeClient(int fd);
	std::string get_name() const;
	void broadcast(int sender_fd, const std::string &msg);
	std::string getTopic() const;			 // b
	void setTopic(const std::string &topic); // b
	std::string getClientList() const;		 // b
	void addOperator(int fd);				 // b
	void removeOperator(int fd);			 // b
	bool isOperator(int fd) const;			 // b
	bool hasClient(int fd) const;			 // b
	void addInvited(int fd);
	bool isInvited(int fd) const;
	void removeInvited(int fd);
	bool isInviteOnly() const;
	bool isTopicRestricted() const;
	std::string getKey() const;
	int getUserLimit() const;
	void setInviteOnly(bool val);
	void setTopicRestricted(bool val);
	void setKey(const std::string &key);
	void setUserLimit(int limit);
	int getClientCount() const;
	std::vector<int> getClientFds() const;
};

#endif
