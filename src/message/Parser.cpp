/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esouhail <esouhail@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/14 22:41:58 by esouhail          #+#    #+#             */
/*   Updated: 2026/03/17 12:25:00 by esouhail         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Message.hpp"
#include "Client.hpp"

#include <cctype>


static void toUpper(std::string &str) {
    for (std::string::size_type i = 0; i < str.size(); ++i)
        str[i] = static_cast<char>(std::toupper(str[i]));
}


bool Parser::parse(const std::string &line, Message &out) {
    out = Message();
    out.raw = line;

    std::string work = line;

    while (!work.empty() && (work[work.size() - 1] == '\n' || work[work.size() - 1] == '\r'))
        work.erase(work.size() - 1);

    if (work.empty())
        return false;

    std::string::size_type trailingPos = work.find(" :");
    if (trailingPos != std::string::npos) {
        out.hasTrailing = true;
        out.trailing = work.substr(trailingPos + 2);
        work = work.substr(0, trailingPos);
    }

    std::vector<std::string> tokens;
    std::string::size_type i = 0;
    while (i < work.size()) {
        while (i < work.size() && work[i] == ' ')
            ++i;
        if (i >= work.size())
            break;
        std::string::size_type start = i;
        while (i < work.size() && work[i] != ' ')
            ++i;
        tokens.push_back(work.substr(start, i - start));
    }

    if (tokens.empty())
        return false;

    out.command = tokens[0];
    toUpper(out.command);

    for (std::vector<std::string>::size_type j = 1; j < tokens.size(); ++j)
        out.params.push_back(tokens[j]);

    return true;
}
