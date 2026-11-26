#include "Server.hpp"
#include "Executor.hpp"
#include "Commands.hpp"

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
    _clients.insert(std::make_pair(clientFd, Client(clientFd, std::string(host))));
    std::cout << "Client "<< clientFd << " connected" << std::endl;
}

void Server::handleClientMessage(int clientFd)
{
    char buffer[1024] = {0};
    ssize_t bytes = recv(clientFd, buffer, sizeof(buffer) -1, 0);
    if( bytes > 0)
    {
        buffer[bytes] = '\0';
        Client& client = _clients[clientFd];
        try {
            client.appendToBuffer(std::string(buffer, bytes));
        }
        catch (const std::exception& e)
        {
            std::cerr<<"Client " <<clientFd << " "<< e.what() << std::endl;
            handleClientDisconnection(clientFd);
            return;
        }
        while(client.hasCompleteCommand())
        {
            std::string cmd = client.extractNextCommand();
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
                handleClientDisconnection(clientFd);
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
	{
        return;
	}
	Client& client = itClient->second;
    for(std::map<std::string, Channel*>::iterator it= _channels.begin(); it != _channels.end();)
    {
        Channel* channel = it->second;
        if(channel->isMember(client))
            channel->removeClient(&client);
        if(channel->isOperator(client))
            channel->removeOperator(client);
        if(channel->isInvited(client))
            channel->removeInvitedClient(client);
        if(channel->isEmpty())
        {
            std::map<std::string, Channel*>::iterator toErase = it; ++it;
            delete toErase->second;
            _channels.erase(toErase);
        }
        else
            ++it;
    }
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
