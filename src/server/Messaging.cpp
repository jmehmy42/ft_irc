#include "Server.hpp"
#include "Executor.hpp"
#include "Commands.hpp"

void	Server::notifySharedChannelsQuit(const Client& client, const std::string& quitMessage)
{
    std::string msg = ":" + client.getNickName() + "!" +
                            client.getUserName() + "@" +
                            client.getHostName() + " QUIT :" +
                            quitMessage + "\r\n"; 
    for(std::map<int, Client>::iterator it = getClients().begin(); it != getClients().end(); it++)
    {
        Client& other = it->second;
        if(other.getFd() == client.getFd())
            continue;
        for(std::map<std::string, Channel*>::iterator ch = getChannels().begin(); ch!= getChannels().end(); ++ch)
        {
            if(ch->second->isMember(client) && ch->second->isMember(other))
            {
                sendMessageToClient(other.getFd(), msg);
                break;
            }
        }
    }
}

void Server::sendMessageToClient(int clientfd, const std::string& message)
{
    std::map<int, Client>::iterator it = _clients.find(clientfd);
    if (it == _clients.end() || message.empty())
        return;
    it->second.getOutputBuffer() += message;
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

void Server::broadcastMessage(const std::string& message, int excludeFd)
{
    for(std::map<int, Client>::iterator it = _clients.begin(); it!= _clients.end(); ++it)
    {
        if(it->first != excludeFd)
            sendMessageToClient(it->first, message);
    }
}

void Server::handleClientWrite(int clientfd)
{
    std::map<int, Client>::iterator it = _clients.find(clientfd);
    if (it == _clients.end())
        return;
    std::string& outputBuffer = it->second.getOutputBuffer();
    if (outputBuffer.empty())
        return;
    ssize_t bytesSent = send(clientfd, outputBuffer.c_str(), outputBuffer.size(), 0);
    if (bytesSent > 0)
        outputBuffer.erase(0, bytesSent);
    else 
    {
        std::cerr << "send failed to client " << clientfd<< std::endl;
        handleClientDisconnection(clientfd);
    }
}