/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:12:11 by marwan            #+#    #+#             */
/*   Updated: 2026/03/14 22:15:41 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client {
  private:
	int _fd;
	std::string _nickname;
	std::string _username;
	bool _registered;
	bool _pass_ok;
	bool _nick_ok;
	bool _user_ok;
	std::string _buffer;
	std::string _outBuffer;

  public:
	Client();
	Client(int fd);
	~Client();
	int get_fd() const;
	void set_nickname(const std::string &nickname);
	std::string get_nickname() const;
	void set_user(const std::string &user);
	void set_pass(bool b);
	void set_nickOK(bool b);
	void set_userOK(bool b);
	void Register();
	bool get_registered() const;
	bool is_registerable() const;
	void set_registered(bool b);
	std::string get_username() const;
	void appendBuffer(const std::string &data);
	std::string extractLine();
	bool hasLine() const;
	void queueOutput(const std::string &data);
	const std::string &getOutputBuffer() const;
	void consumeOutput(size_t count);
	bool hasPendingOutput() const;
};

#endif
