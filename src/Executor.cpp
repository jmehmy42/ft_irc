/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 03:49:11 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/14 14:52:04 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Commands.hpp"

#include <sstream>
#include <cctype>

Executor::Executor()
{
}

void Executor::execute(Server& server, Client& client,
                              const std::string& message)
{
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if(command.empty())
        return;
    for (size_t i = 0; i < command.size(); ++i)
        command[i] = std::toupper(command[i]);
    
    if (command == "PASS")
        executePass(server, client, iss);
    else if (command == "NICK")
        executeNick(server, client, iss);
    else if (command == "USER")
        executeUser(server, client, iss);
    else if(command == "JOIN")
        executeJoin(server, client, iss);
    else if (command == "PRIVMSG")
        executePrivmsg(server, client, iss);
    else if(command == "PART")
       executePart(server, client, iss);
    else if (command == "KICK")
        executeKick(server, client, iss);
    else if(command == "TOPIC")
        executeTopic(server, client, iss);
    else
        sendMessage(server, client, 421, "unknown command");
        // else if(command == "CAP")
    //      executeCap(server, client, iss);
    
    
    // else if(command == "INVITE")
    //     executeInvite(server, client, iss);
    // else if(command == "MODE")
    //     executeMode(server, client, iss)
    // else if(command == "PART")
    //    executePart(server, client, iss);
    // else if (command == "PING")
    //     executePing(server, client, iss);
    // else if (command == "QUIT")
    //     executeQuit(server, client, iss);
    // else if(command == "PONG")
        return;
    
}
