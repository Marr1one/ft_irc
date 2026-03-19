/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 21:55:03 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 21:15:54 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: _fd(-1), _registered(false), _pass_ok(false), _nick_ok(false),
	  _user_ok(false) {}

Client::Client(int fd)
	: _fd(fd), _registered(false), _pass_ok(false), _nick_ok(false),
	  _user_ok(false) {}

Client::~Client() {}

int Client::get_fd() const { return this->_fd; }

std::string Client::get_nickname() const { return this->_nickname; }

void Client::set_user(const std::string &user) { this->_username = user; }

void Client::set_nickname(const std::string &nickname) {
	this->_nickname = nickname;
}

void Client::set_pass(bool b) { this->_pass_ok = b; }

void Client::set_nickOK(bool b) { this->_nick_ok = b; }

void Client::set_userOK(bool b) { this->_user_ok = b; }

void Client::Register() { _registered = true; }

bool Client::get_registered() const { return this->_registered; }

bool Client::is_registerable() const {
	return (_pass_ok && _nick_ok && _user_ok);
}

void Client::set_registered(bool b) { _registered = b; }
void Client::appendBuffer(
	const std::string &data) // ajoute au buff ce qu on viens de recevoir
{
	_buffer += data;
}
bool Client::hasLine() const // verification si il a une ligne complete
{
	return (_buffer.find('\n') != std::string::npos);
}
std::string Client::extractLine() // Extrait et retourne la première ligne
								  // complète du buffer
{
	size_t pos = _buffer.find('\n');
	if (pos == std::string::npos)
		return "";

	std::string line = _buffer.substr(0, pos);
	_buffer = _buffer.substr(pos + 1);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	return (line);
}

std::string Client::get_username() const { return this->_username; }

void Client::queueOutput(const std::string &data) { _outBuffer += data; }

const std::string &Client::getOutputBuffer() const { return _outBuffer; }

void Client::consumeOutput(size_t count) { _outBuffer.erase(0, count); }

bool Client::hasPendingOutput() const { return !_outBuffer.empty(); }
