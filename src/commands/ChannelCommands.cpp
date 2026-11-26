#include "Server.hpp"
#include "Client.hpp"
#include "Executor.hpp"

void executeJoin(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    std::string key;
    iss >> channelName >> key;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    if (channelName.empty())
    {
        sendMessage(server, client, 461, "JOIN :Not enough parameters");
        return;
    }
    for(size_t i = 0; i < channelName.size(); ++i)
    {
        if( channelName[i] == ',' || channelName[i] == ':' || channelName[i] == ' ' 
            || channelName[0] != '#' || channelName.length() < 2 || channelName.length() > 50)
            {
                sendMessage(server, client, 403, channelName + " :No such channel");
                return;
            }
    }
    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);
    Channel* channel;
    if (it == server.getChannels().end())
    {
        server.createChannel(channelName, client);
        it = server.getChannel(channelName);
        channel = it->second;
    }
    else
    {
        channel = it->second;

        if (channel->isMember(client))
            return;
        if (channel->isInvitedOnly() && !channel->isInvited(client))
        {
            sendMessage(server, client, 473, channelName + " :Cannot join channel (+i)");
            return;
        }
        if (channel->isPasswordProtected() && key != channel->getPassword())
        {
            sendMessage(server, client, 475, channelName + " :Cannot join channel (+k)");
            return;
        }
        if (channel->getUserLimit() > 0
            && channel->getMemberCount() >= channel->getUserLimit())
        {
            sendMessage(server, client, 471, channelName + " :Cannot join channel (+l)");
            return;
        }
        channel->addMember(client);
        channel->removeInvitedClient(client);
    }
    std::string message =
        ":" + client.getNickName()
        + "!" + client.getUserName()
        + "@" + client.getHostName()
        + " JOIN :" + channelName + "\r\n";
    std::vector<Client*> members = channel->getMembers();
    for (std::vector<Client*>::iterator member = members.begin();
         member != members.end(); ++member)
    {
        server.sendMessageToClient((*member)->getFd(), message);
    }
}

void executePart(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    std::string reason;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    iss >> channelName;
    if (channelName.empty())
    {
        sendMessage(server, client, 461, "PART :Not enough parameters");
        return;
    }
    std::getline(iss, reason);
    while (!reason.empty() && (reason[0] == ' ' || reason[0] == '\t'))
        reason.erase(0, 1);
    if (!reason.empty() && reason[reason.size() - 1] == '\r')
        reason.erase(reason.size() - 1);
    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);
    if (reason.empty())
        reason = "Leaving";
    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);
    if (it == server.getChannels().end())
    {
        sendMessage(server, client, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = it->second;
    if (!channel->isMember(client))
    {
        sendMessage(server, client, 442, channelName + " :You're not on that channel");
        return;
    }
    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " PART " + channelName + " :" + reason + "\r\n";
    std::vector<Client*> members = channel->getMembers();
    for (std::vector<Client*>::iterator member = members.begin();
         member != members.end(); ++member)
    {
        server.sendMessageToClient((*member)->getFd(), message);
    }
    channel->removeMember(client);
    channel->removeOperator(client);
    channel->assignNewOperator();
    if (channel->isEmpty())
        server.removeChannel(channelName);
}

void executeKick(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    std::string nickname;
    std::string reason;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    iss >> channelName >> nickname;
    if (channelName.empty() || nickname.empty())
    {
        sendMessage(server, client, 461, "KICK :Not enough parameters");
        return;
    }
    std::getline(iss, reason);
    while (!reason.empty() && (reason[0] == ' ' || reason[0] == '\t'))
        reason.erase(0, 1);

    if (!reason.empty() && reason[reason.size() - 1] == '\r')
        reason.erase(reason.size() - 1);

    if (!reason.empty() && reason[0] == ':')
        reason.erase(0, 1);

    if (reason.empty())
        reason = client.getNickName();

    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);

    if (it == server.getChannels().end())
    {
        sendMessage(server, client, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = it->second;
    if (!channel->isMember(client))
    {
        sendMessage(server, client, 442, channelName + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(client))
    {
        sendMessage(server, client, 482, channelName + " :You're not channel operator");
        return;
    }
    Client* target = server.getClientByNickname(nickname);
    if (target == NULL)
    {
        sendMessage(server, client, 401, nickname + " :No such nick");
        return;
    }
    if (!channel->isMember(*target))
    {
        sendMessage(server, client, 441,
            nickname + " " + channelName + " :They aren't on that channel");
        return;
    }
    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " KICK " + channelName + " " +
        nickname + " :" + reason + "\r\n";
    std::vector<Client*> members = channel->getMembers();
    for (std::vector<Client*>::iterator member = members.begin();
         member != members.end(); ++member)
    {
        server.sendMessageToClient((*member)->getFd(), message);
    }
    channel->removeMember(*target);
    channel->removeOperator(*target);
    if (channel->isEmpty())
        server.removeChannel(channelName);
}

void executeInvite(Server& server, Client& client, std::istringstream& iss)
{
    std::string nickname;
    std::string channelName;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    iss >> nickname >> channelName;
    if (nickname.empty() || channelName.empty())
    {
        sendMessage(server, client, 461, "INVITE :Not enough parameters");
        return;
    }
    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);
    if (it == server.getChannels().end())
    {
        sendMessage(server, client, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = it->second;
    if (!channel->isMember(client))
    {
        sendMessage(server, client, 442, channelName + " :You're not on that channel");
        return;
    }
    Client* target = server.getClientByNickname(nickname);
    if (target == NULL)
    {
        sendMessage(server, client, 401, nickname + " :No such nick");
        return;
    }
    if (channel->isMember(*target))
    {
        sendMessage(server, client, 443,
            nickname + " " + channelName + " :is already on channel");
        return;
    }
    if (channel->isInvitedOnly() && !channel->isOperator(client))
    {
        sendMessage(server, client, 482,
            channelName + " :You're not channel operator");
        return;
    }
    channel->addInvitedClient(*target);
    sendMessage(server, client, 341, nickname + " " + channelName);
    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " INVITE " + nickname + " :" + channelName + "\r\n";
    server.sendMessageToClient(target->getFd(), message);
}

void executeTopic(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    std::string topic;
    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }
    iss >> channelName;
    if (channelName.empty())
    {
        sendMessage(server, client, 461, "TOPIC :Not enough parameters");
        return;
    }
    std::getline(iss, topic);
    while (!topic.empty() && (topic[0] == ' ' || topic[0] == '\t'))
        topic.erase(0, 1);
    if (!topic.empty() && topic[topic.size() - 1] == '\r')
        topic.erase(topic.size() - 1);
    bool changingTopic = !topic.empty();
    if (!topic.empty() && topic[0] == ':')
        topic.erase(0, 1);
    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);
    if (it == server.getChannels().end())
    {
        sendMessage(server, client, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = it->second;
    if (!channel->isMember(client))
    {
        sendMessage(server, client, 442, channelName + " :You're not on that channel");
        return;
    }
    if (!changingTopic)
    {
        if (channel->getTopic().empty())
        {
            sendMessage(server, client, 331, channelName + " :No topic is set");
        }
        else
        {
            sendMessage(server, client, 332, channelName + " :" + channel->getTopic());
        }
        return;
    }
    if (channel->isTopicRestricted() && !channel->isOperator(client))
    {
        sendMessage(server, client, 482, channelName + " :You're not channel operator");
        return;
    }
    channel->setTopic(topic);
    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " TOPIC " + channelName + " :" + topic + "\r\n";
    std::vector<Client*> members = channel->getMembers();
    for (std::vector<Client*>::iterator member = members.begin();
         member != members.end(); ++member)
    {
        server.sendMessageToClient((*member)->getFd(), message);
    }
}

void executeNames(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    iss >> channelName;
    std::map<std::string, Channel*>& channels = server.getChannels();
    if (!channelName.empty())
    {
        std::map<std::string, Channel*>::iterator it =
            channels.find(channelName);
        if (it == channels.end())
            return;
        Channel* channel = it->second;
        std::vector<Client*> members = channel->getMembers();
        std::string names;
        for (std::vector<Client*>::iterator m = members.begin();
             m != members.end(); ++m)
        {
            if (client.hasCapability("multi-prefix"))
            {
                if (channel->isOperator(**m))
                    names += "@";
                if (channel->isVoice(**m))
                    names += "+";
            }
            else
            {
                if (channel->isOperator(**m))
                    names += "@";
                else if (channel->isVoice(**m))
                    names += "+";
            }
            names += (*m)->getNickName() + " ";
        }
        server.sendMessageToClient(client.getFd(),
            ":localhost 353 " + client.getNickName()
            + " = " + channelName + " :" + names + "\r\n");
        server.sendMessageToClient(client.getFd(),
            ":localhost 366 " + client.getNickName()
            + " " + channelName + " :End of /NAMES list\r\n");
        return;
    }
    for (std::map<std::string, Channel*>::iterator it = channels.begin();
         it != channels.end(); ++it)
    {
        Channel* channel = it->second;
        std::vector<Client*> members = channel->getMembers();
        std::string names;
        for (std::vector<Client*>::iterator m = members.begin();
             m != members.end(); ++m)
        {
            if (client.hasCapability("multi-prefix"))
            {
                if (channel->isOperator(**m))
                    names += "@";
                if (channel->isVoice(**m))
                    names += "+";
            }
            else
            {
                if (channel->isOperator(**m))
                    names += "@";
                else if (channel->isVoice(**m))
                    names += "+";
            }
            names += (*m)->getNickName() + " ";
        }
        server.sendMessageToClient(client.getFd(),
            ":localhost 353 " + client.getNickName()
            + " = " + channel->getName() + " :" + names + "\r\n");
    }
    server.sendMessageToClient(client.getFd(),
        ":localhost 366 " + client.getNickName()
        + " * :End of /NAMES list\r\n");
}

