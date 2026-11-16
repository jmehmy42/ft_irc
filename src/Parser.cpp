/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 03:38:48 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/14 14:51:45 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include <cctype>

bool Parser::parse(const std::string& line, IrcMessage& message)
{
    message.prefix.clear();
    message.command.clear();
    message.params.clear();
    std::string input = line;
    while (!input.empty() &&
           (input[input.size() - 1] == '\r' ||
            input[input.size() - 1] == '\n'))
    {
        input.erase(input.size() - 1);
    }
    if (input.empty())
        return false;
    std::string::size_type pos = 0;
    while (pos < input.size() && input[pos] == ' ')
        ++pos;
    if (pos == input.size())
        return false;
    if (input[pos] == ':')
    {
        ++pos;
        std::string::size_type end = input.find(' ', pos);
        if (end == std::string::npos || end == pos)
            return false;
        message.prefix = input.substr(pos, end - pos);
        pos = end;
        while (pos < input.size() && input[pos] == ' ')
            ++pos;
    }
    if (pos == input.size())
        return false;
    std::string::size_type command_end = input.find(' ', pos);
    if (command_end == std::string::npos)
        command_end = input.size();
    message.command = input.substr(pos, command_end - pos);
    if (message.command.empty())
        return false;
    for (std::string::size_type i = 0;
         i < message.command.size(); ++i)
    {
        message.command[i] = std::toupper(
            static_cast<unsigned char>(message.command[i])
        );
    }
    pos = command_end;
    while (pos < input.size())
    {
        while (pos < input.size() && input[pos] == ' ')
            ++pos;
        if (pos == input.size())
            break;
        if (message.params.size() == 15)
            return false;
        if (input[pos] == ':')
        {
            ++pos;
            message.params.push_back(input.substr(pos));
            break;
        }
        std::string::size_type end = input.find(' ', pos);
        if (end == std::string::npos)
            end = input.size();
        message.params.push_back(input.substr(pos, end - pos));
        pos = end;
    }
    return true;
}