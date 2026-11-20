/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 09:38:12 by jmehmy            #+#    #+#             */
/*   Updated: 2026/11/20 14:03:01 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//throw std::runtime_error("Failed to create socket");

static bool CheckInvalidPort(int port)
{
    if (port < 1 || port > 65535)
    {
        std::cerr << "Error: Invalid Port Range (1 to 65535)"
                  << std::endl;
        return true;
    }
    return false;
}

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: ./ircserv <port> <password>"
                  << std::endl;
        return 1;
    }
    int port = std::atoi(av[1]);

    if (CheckInvalidPort(port))
        return 1;
    try
    {
        Server server(port, av[2]);
        
        // Client testClient(100, "127.0.0.1");
        // testClient.setNickName("Jas");

        // server.getClients()[100] = testClient;
        // std::cout << "\n=== TEST getClients ===" << std::endl;
        // std::map<int, Client>& clients = server.getClients();
        // std::cout << "Number of clients: " << clients.size() << std::endl;
        // std::cout << "Nickname: " << clients[100].getNickName() << std::endl;
        // std::cout << "\n=== TEST const getClients() ===" << std::endl;
        // const Server& constServer = server;
        // const std::map<int, Client>& constClients = constServer.getClients();
        // std::cout << "Number of clients: " << constClients.size() << std::endl;
        // // Test getClientByNickname()
        // std::cout << "\n=== TEST getClientByNickname() ===" << std::endl;
        // Client* found = server.getClientByNickname("jas");
        // if (found != NULL)
        //     std::cout << "Client found: " << found->getNickName() << std::endl;
        // else
        //     std::cout << "Client not found!" << std::endl;
        // // Test nickname that does not exist
        // found = server.getClientByNickname("bob");
        // if (found != NULL)
        //     std::cout << "Client found: " << found->getNickName() << std::endl;
        // else
        //     std::cout << "Bob not found!" << std::endl;
        // std::cout << "\n=== CREATE CHANNEL ===" << std::endl;
        // server.createChannel("#42Lisboa", testClient);
        // std::map<std::string, Channel*>::iterator it;
        // it = server.getChannel("#42Lisboa");
        // if (it != server.getChannels().end())
        // {
        //     std::cout << "Channel created: " << it->second->getName() << std::endl;
        //     std::cout << "Members: " << it->second->getMemberCount() << std::endl;
        //     std::cout << "Is operator: " << it->second->isOperator(testClient) << std::endl;
        // }
        // std::cout << "\n=== REMOVE CHANNEL ===" << std::endl;
        // server.removeChannel("#42Lisboa");
        // if (server.getChannel("#42Lisboa") == server.getChannels().end())
        //     std::cout << "Channel successfully removed!" << std::endl;
        // else
        //     std::cout << "Channel still exists!" << std::endl;
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}