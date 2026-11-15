/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 21:33:39 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/14 14:50:33 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Parser.hpp"
#include "Executor.hpp"

Server* g_server = NULL;

static void handleSignal(int signum);

Server::Server(int port, const std::string& pass)
    : _port(port), _password(pass), _fd(-1), _running(true)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd == -1)
		throw std::runtime_error("socket creation failed!");

    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to set socket options");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to bind socket");
    }
    if (listen(_fd, SOMAXCONN) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to listen");
    }
    
    std::cout << "Server listening on port "
              << _port << std::endl;
    
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
	
}

Server::~Server()
{
    // for(std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    //     delete it->second;
    _clients.clear();
    if (_fd != -1)
        close(_fd);
}

void Server::closeSocketFd()
{
    if(_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

void Server::run()
{
    struct pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);
    while (_running)
    {
        int activity = poll(_pollfds.data(), _pollfds.size(), -1);
        if( activity < 0)
        {
            if(errno == EINTR)
                break;
            throw std::runtime_error("Poll failed!");
            break;
        }
        if (_pollfds[0].revents & POLLIN)
            handleNewConnection();
        for (size_t i = 1; i < _pollfds.size(); ++i)
        {
            if(_pollfds[i].revents & POLLIN)
            {
                handleClientMessage(_pollfds[i].fd);
            }
        }
        // for (std::vector<Client*>::iterator it = _clients.begin();
        //      it != _clients.end(); ++it)
        // {
        //     struct pollfd client_poll;
        //     client_poll.fd = (*it)->getFd();
        //     client_poll.events = POLLIN;
        //     if (!(*it)->getOutputBuffer().empty())
        //         client_poll.events |= POLLOUT;
        //     client_poll.revents = 0;

        //     fds.push_back(client_poll);
        // }
        // int ret = poll(&fds[0], fds.size(), -1);
        // if (ret == -1)
        // {
        //     if (errno == EINTR)
        //         continue;

        //     throw std::runtime_error("poll failed");
        // }
        // for (int i = static_cast<int>(fds.size()) - 1; i >= 1; --i)
        // {
        //     if (fds[i].revents == 0)
        //         continue;
        //     Client* client = _clients[i - 1];

        //     if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL))
        //     {
        //         std::cout << "Client disconnected: "
        //                 << client->getFd() << std::endl;

        //         delete client;
        //         _clients.erase(_clients.begin() + (i - 1));
        //         continue;
        //     }
        //     if (fds[i].revents & POLLIN)
        //     {
        //         char buffer[512];

        //         int bytes = recv(client->getFd(), buffer, sizeof(buffer), 0);

        //         if (bytes == 0)
        //         {
        //             std::cout << "Client disconnected: "
        //                     << client->getFd() << std::endl;

        //             delete client;
        //             _clients.erase(_clients.begin() + (i - 1));
        //             continue;
        //         }

        //         if (bytes > 0)
        //         {
        //             client->appendBuffer(std::string(buffer, bytes));

        //             while (client->hasCompleteMessage())
        //             {
        //                 std::string line = client->getNextMessage();

        //                 IrcMessage message;

        //                 if (Parser::parse(line, message))
        //                 {
        //                     std::string response =
        //                         _executor.execute(*client, message);

        //                     if (!response.empty())
        //                         client->appendOutput(response);
        //                 }
        //             }
        //         }
        //     }
        //     if (fds[i].revents & POLLOUT)
        //     {
        //         std::string& output = client->getOutputBuffer();

        //         if (!output.empty())
        //         {
        //             int sent = send(client->getFd(),
        //                             output.c_str(),
        //                             output.size(),
        //                             0);

        //             if (sent > 0)
        //                 output.erase(0, sent);
        //             else if (sent == 0)
        //             {
        //                 delete client;
        //                 _clients.erase(_clients.begin() + (i - 1));
        //                 continue;
        //             }
        //         }
        //     }
        // }
        // if (fds[0].revents & POLLIN)
        // {
        //     struct sockaddr_in client_addr;
        //     socklen_t client_len = sizeof(client_addr);

        //     int client_fd = accept(
        //         _fd,
        //         (struct sockaddr*)&client_addr,
        //         &client_len
        //     );

        //     if (client_fd != -1)
        //     {
        //         if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
        //         {
        //             close(client_fd);
        //             throw std::runtime_error(
        //                 "Failed to set client non-blocking mode"
        //             );
        //         }

        //         Client* new_client = new Client(client_fd);
        //         _clients.push_back(new_client);

        //         std::cout << "New client connected: "
        //                 << inet_ntoa(client_addr.sin_addr)
        //                 << " (fd: " << client_fd << ")"
        //                 << std::endl;
        //     }
        //}
    }
}

void Server::handleNewConnection()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_fd, (struct sockaddr*)&clientAddr, &clientLen);
    if(clientFd == -1)
    {
        throw std::runtime_error("Accept failed!");
        return ;
    }

    if(fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("fcntl failed!");
    }
    struct pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);

    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, host, INET_ADDRSTRLEN);
    Client newClient(clientFd, std::string(host));

    _clients[clientFd] = newClient;

    std::cout << "Client "<< clientFd << " connected" << std::endl;
}

void Server::handleClientMessage(int clientFd)
{
    char buffer[1024] = {0};
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer) -1, 0);
    if( bytes > 0)
    {
        buffer[bytes] = '\0';
        //std::cout << "Received from client " << clientFd << ": " << buffer << std::endl;
        Client& client = _clients[clientFd];

        client.appendToBuffer(buffer);
        while(client.hasCompleteCommand())
        {
            std::string cmd = client.extractNextCommand();
            //std::cout << "Extracted command: [" << cmd << "]" << std::endl;
            _executor.execute(*this, client, cmd);
            if(_clients.find(clientFd) == _clients.end())
                break;
        }
    }
    else
    {
        if(bytes == 0)
        {
            std::map<int, Client>::iterator it = _clients.find(clientFd);
            if(it != _clients.end())
            {
                handleClientDisconnection(clientFd);
            }
        }
        else
            std::cout << "Recieve failed from client "<< clientFd << std::endl;
    }
}

void Server::handleClientDisconnection(int clientFd)
{
    std::cout << "Client "<< clientFd << " disconnected" << std::endl;
    std::map <int, Client> :: iterator itClient = _clients.find(clientFd);
    if(itClient == _clients.end())
        return;
    
    for(size_t i = 0; i < _pollfds.size(); ++i)
    {
        if(_pollfds[i].fd == clientFd)
        {
            _pollfds.erase(_pollfds.begin() + i);
            break;
        }
    }

    close(clientFd);
    _clients.erase(itClient);
}

static void handleSignal(int signum)
{
    if(g_server)
        g_server->signalHandler(signum);
}
void Server::signalHandler(int signum)
{
    (void)signum;
    _running = false;
    std::cout << "Signal interrupt! "<< signum << "Server is shutting down." << std::endl;
}

const std::string& Server::getPassword()const
{
    return _password;
}

bool Server::isNickNameTaken(const std::string& nickname) const
{
    for(std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if(it->second.getNickName() == nickname)
            return true;
    }
    return false;
}


void Server::sendMessageToClient(int clientfd, const std::string& message)
{
    ssize_t bytesent = send(clientfd, message.c_str(), message.length(), 0);
    if(bytesent == -1)
        std::cerr << "send failed to client " << clientfd<< std::endl;
}
void sendMessage(Server& server, Client& client, int code, const std::string& text)
{
    std::ostringstream oss;
    oss << ":ircserv ";
    oss.width(3);
    oss.fill('0');
    oss << code;
    oss << " "
    << (client.getNickName().empty() ? "*" : client.getNickName()) << " :" << text << "\r\n";
    server.sendMessageToClient(client.getFd(), oss.str());
}

