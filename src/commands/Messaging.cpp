#include "Server.hpp"
#include "Client.hpp"
#include "Executor.hpp"

void executePrivmsg(Server& server, Client& client, std::istringstream& iss)
{
    std::string target;
    std::string message;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    iss >> target;
    if (target.empty())
    {
        sendMessage(server, client, 411, "No recipient given (PRIVMSG)");
        return;
    }
    std::getline(iss, message);
    while (!message.empty() && (message[0] == ' ' || message[0] == '\t'))
        message.erase(0, 1);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);
    if (!message.empty() && message[message.size() - 1] == '\r')
        message.erase(message.size() - 1);
    if (message.empty())
    {
        sendMessage(server, client, 412, "No text to send");
        return;
    }
    std::string formattedMessage =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " PRIVMSG " + target + " :" + message + "\r\n";
    if (target[0] == '#')
    {
        std::map<std::string, Channel*>::iterator it =
            server.getChannel(target);

        if (it == server.getChannels().end())
        {
            sendMessage(server, client, 403, target + " :No such channel");
            return;
        }
        Channel* channel = it->second;
        if (!channel->isMember(client))
        {
            sendMessage(server, client, 404,
                target + " :Cannot send to channel");
            return;
        }
        std::vector<Client*> members = channel->getMembers();
        for (std::vector<Client*>::iterator member = members.begin();
             member != members.end(); ++member)
        {
            if ((*member)->getFd() != client.getFd())
                server.sendMessageToClient((*member)->getFd(), formattedMessage);
        }
    }
    else
    {
        Client* recipient = server.getClientByNickname(target);
        if (recipient == NULL)
        {
            sendMessage(server, client, 401, target + " :No such nick");
            return;
        }
        server.sendMessageToClient(recipient->getFd(), formattedMessage);
    }
}

