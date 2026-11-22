#include "Server.hpp"
#include "Executor.hpp"
#include "Commands.hpp"

std::map<int, Client>& Server::getClients()
{
    return _clients;
}

const std::map<int, Client>& Server::getClients() const
{
    return _clients;
}

Client* Server::getClientByNickname(const std::string& nickname)
{
    for(std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
    {
        if(it->second.getNickName() == nickname)
            return &it->second;
    }
    return NULL;
}

bool Server::isNickNameTaken(const std::string& nickname, int clientfd) const
{
    std::string str = nickname;
    for(size_t i = 0; i < str.size(); i++)
        str[i]= std::tolower(static_cast<unsigned char>(str[i]));        
    for(std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it-> first == clientfd)
            continue;
        if(it->second.getNickName() == str)
            return true;
    }
    return false;
}
