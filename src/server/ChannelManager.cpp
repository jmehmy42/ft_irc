#include "Server.hpp"
#include "Executor.hpp"
#include "Commands.hpp"

std::map<std::string, Channel*>& Server::getChannels()
{
    return _channels;
}

std::map<std::string, Channel*>::iterator Server::getChannel(const std::string& name)
{
    return _channels.find(name);
}

void Server::createChannel(const std::string& name, Client& newClient)
{
    if(_channels.find(name) != _channels.end())
        return ;
    Channel* newChannel = new Channel(name, newClient);
    _channels[name] = newChannel;
}

void Server::removeChannel(const std::string& name)
{
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if(it != _channels.end())
    {
        delete it->second;
        _channels.erase(it);
    }
}