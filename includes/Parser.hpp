/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 03:37:03 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/11 03:38:12 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

struct IrcMessage
{
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
};

class Parser
{
    public:
        static bool parse(const std::string& line, IrcMessage& message);
};

#endif