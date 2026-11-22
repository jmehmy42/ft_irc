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

static bool CheckInvalidPort(const std::string& str)
{
    if(str.empty())
    {
        std::cerr << "Error: invalid port" << std::endl;
        return true;
    }
    for(size_t i = 0; i < str.size(); i++)
    {
        if(str[i] < '0' || str[i] > '9')
        {
            std::cerr << "Error: port must contain only digits" << std::endl;
            return true;
        }
    }
    if(str.length() > 5)
    {
        std::cerr << "Error: Invalid Port Range (1 to 65535)"
                  << std::endl;
        return true;
    }
    int port = std::atoi(str.c_str());
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
    if (CheckInvalidPort(av[1]))
        return 1;
    int port = std::atoi(av[1]);
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
