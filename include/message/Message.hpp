/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 18:27:19 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/14 18:34:24 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <string>
#include <vector>

struct Message {
	std::string raw;
	std::string command;
	std::vector<std::string> params;
	std::string trailing;
	bool hasTrailing;

	Message() : hasTrailing(false) {}
};

#endif