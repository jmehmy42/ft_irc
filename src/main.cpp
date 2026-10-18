/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmehmy <jmehmy@student.42lisboa.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/27 09:38:12 by jmehmy            #+#    #+#             */
/*   Updated: 2026/08/16 17:21:29 by jmehmy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static bool CheckInvalidPort(int port)
{
	if(port < 1 || port > 65535)
	{
		std::cerr << "Error: Invalid Port Range(1 to 65535)" << std::endl;
		return true;
	}
	return false;
}

int main(int ac, char **av) 
{
	
	if(ac != 3)
	{
		std::cerr << "Use: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	int port = std::atoi(av[1]);
	if(CheckInvalidPort(port))
		return 1;
	try{
		//run server with av[2];
	}
	catch(const std::exception& e)
	{
		std::cerr<<"Error: "<< e.what()<< std::endl;
		return 1;
	}
		
	return 0;
}
