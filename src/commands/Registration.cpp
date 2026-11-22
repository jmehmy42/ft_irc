#include "Server.hpp"
#include "Client.hpp"
#include "Executor.hpp"

void executePass(Server &server, Client &client, std::istringstream &iss)
{
    std::string password;
    iss >> password;
    if (client.getIsRegistered())
    {
        sendMessage(server, client, 462, "You may not reregister");
        return;
    }
    if (password.empty())
    {
        sendMessage(server, client, 461, "PASS not enough parameters");
        return;
    }
    if (password != server.getPassword())
    {
        client.setHasPass(false);
        sendMessage(server, client, 464, "Password incorrect");
        return;
    }
    client.setPassWord(password);
    client.setHasPass(true);
}

void executeNick(Server &server, Client &client, std::istringstream &iss)
{
    std::string nickname;
    iss >> nickname;
    if (!client.getHasPass())
    {
        sendMessage(server, client, 464, "Password required");
        return;
    }
    if (nickname.empty())
    {
        sendMessage(server, client, 431, "No nickname given");
        return;
    }
    unsigned char first = static_cast<unsigned char>(nickname[0]);
    if(nickname.length() > 9 || !std::isalpha(first))
    {
        sendMessage(server, client, 432, nickname + " Erroneous nickname");
        return;
    }
    for (std::string::size_type i = 0; i < nickname.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(nickname[i]);
    
        if (!std::isalnum(c) && c != '-' && c != '_' )
        {
            sendMessage(server, client, 432, nickname + " Erroneous nickname");
            return;
        }
    }
    if (server.isNickNameTaken(nickname, client.getFd()) == true)
    {
        sendMessage(server, client, 433, nickname + " Nickname is already in use");
        return;
    }
    client.setNickName(nickname);
    client.setHasNick(true);
    if (!client.getIsRegistered() && client.getHasPass() && client.getHasNick() && client.getHasUser())
    {
        client.setIsRegistered(true);
        std::cout << "Client "<< client.getFd()<<" " << client.getNickName() << " is registered" << std::endl;
        sendMessage(server, client, 001, "Welcome to the IRC server!");
        return;
    }
}

void executeUser(Server &server, Client &client, std::istringstream &iss)
{
    if (client.getIsRegistered())
    {
        sendMessage(server, client, 462, "You may not reregister");
        return;
    }
    if (!client.getHasPass())
    {
        sendMessage(server, client, 464, "Password required");
        return;
    }
    if (client.getHasUser())
    {
        sendMessage(server, client, 462, "You may not reregister");
        return;
    }
    std::string username, hostname, servername, realname;
    iss >> username >> hostname >> servername;
    std::getline(iss, realname);
    bool hasColon = false;
    if (!realname.empty() && realname[0] == ' ')
        realname.erase(0, 1);
    if (!realname.empty() && realname[0] == ':')
    {
        hasColon = true;
        realname.erase(0, 1);
    }
    if (username.empty() || hostname.empty() ||
        servername.empty() || realname.empty() ||
        (!hasColon&& realname.find(' ') != std::string::npos))
    {
        sendMessage(server, client, 461, "Not enough parameters");
        return;
    }
    client.setUserName(username);
    client.setRealName(realname);
    client.setHasUser(true);

    if (client.getHasPass() && client.getHasNick() && client.getHasUser())
    {
        client.setIsRegistered(true);
        std::cout << "Client "<< client.getFd()<<" " << client.getNickName() << " is registered" << std::endl;
        sendMessage(server, client, 001, "Welcome to the IRC server!");
        return;
    }
}