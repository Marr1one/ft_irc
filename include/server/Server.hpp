/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:17:48 by marwan            #+#    #+#             */
/*   Updated: 2026/03/17 21:03:46 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"

struct Message;

class Server {
  private:
	typedef void (Server::*CommandFn)(int, const Message &);

	typedef std::vector<pollfd>::iterator PollFdIt;
	typedef std::map<int, Client>::iterator ClientIt;
	typedef std::map<int, Client>::const_iterator ConstClientIt;
	typedef std::map<std::string, Channel>::iterator ChannelIt;
	typedef std::map<std::string, CommandFn>::iterator HandlerIt;

	int _port;
	std::string _password;
	int _serverFd;
	std::vector<pollfd> _fds;
	std::map<int, Client> _clients;
	std::map<std::string, Channel> _channels;
	std::map<std::string, CommandFn> _handlers;

	static bool _running;

  private:
	void initCommandHandlers();
	bool isAllowedBeforeRegistration(const std::string &command) const;
	void handleMessage(int fd, const Message &msg);

	void acceptClient();
	void receiveMessage(int index);
	void flushPendingWrites(int fd);
	void removeClient(int fd);
	void removeClientFromPoll(int fd);
	void updatePollEvents(int fd);
	void sendReply(int fd, const std::string &reply);
	void sendUserMsg(int fd, const std::string &target, const std::string &msg);
	void sendChannelMsg(int fd, const std::string &channelName,
						const std::string &msg);

	bool checkNickname(const std::string &nickname) const;
	bool checkUsername(const std::string &username) const;
	int findClientByNick(const std::string &nick) const;
	bool canFinishRegistration(const Client &client) const;

	void joinChannel(int fd, const std::string &name);
	void partChannel(int fd, const std::string &name);

	void handlePass(int fd, const Message &msg);
	void handleNick(int fd, const Message &msg);
	void handleUser(int fd, const Message &msg);
	void handleJoin(int fd, const Message &msg);
	void handlePrivmsg(int fd, const Message &msg);
	void handlePart(int fd, const Message &msg);
	void handleQuit(int fd, const Message &msg);
	void handleKick(int fd, const Message &msg);
	void handleInvite(int fd, const Message &msg);
	void handleTopic(int fd, const Message &msg);
	void handleMode(int fd, const Message &msg);
	void handlePing(int fd, const Message &msg);
	void handleCap(int fd, const Message &msg);

	static void signalHandler(int signal);

  public:
	Server(char *port, char *password);
	~Server();

	void start();
};

#endif
