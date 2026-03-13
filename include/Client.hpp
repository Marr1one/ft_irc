/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: braugust <braugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:12:11 by marwan            #+#    #+#             */
/*   Updated: 2026/03/09 12:47:24 by braugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#ifndef CLIENT_HPP
#define CLIENT_HPP

class Client
{
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        bool _registered;
        bool _pass_ok;
        bool _nick_ok;
        bool _user_ok;
        std::string _buffer; //b
    public:
        Client();
        Client(int fd);
        ~Client();
        int get_fd()const;
        void set_nickname(const std::string &nickname);
        std::string get_nickname()const;
        void set_user(const std::string &user);
        void set_pass(bool b);
        void set_nickOK(bool b);
        void set_userOK(bool b);
        void Register();
        bool get_registered() const;
        bool is_registerable()const;
        void set_registered(bool b);
        std::string get_username() const; //b
        void        appendBuffer(const std::string &data); //b
        std::string extractLine(); //b
        bool        hasLine() const; //b
};


#endif