/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Executor.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/11 03:48:54 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/11 03:48:55 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <string>

class Server;
class Client;

class Executor
{
    
    public:
        Executor();

        void execute (Server& server, Client& client,
                            const std::string& message);
};

#endif