/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 09:38:12 by jmehmy            #+#    #+#             */
/*   Updated: 2026/11/14 14:51:56 by kwillian         ###   ########.fr       */
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
        server.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}