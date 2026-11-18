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
        sendMessage(server, client, 461, "PASS :Not enough parameters");
        return;
    }
    if (password != server.getPassword())
    {
        sendMessage(server, client, 464, "Password incorrect");
        return;
    }

    client.setPassWord(password);
    client.setHasPass(true);
}

// NICK

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

    for (std::string::size_type i = 0; i < nickname.size(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(nickname[i]);

        if (!std::isalnum(c) && c != '-' && c != '_')
        {
            sendMessage(server, client, 432, nickname + "Erroneous nickname");
            return;
        }
    }

    if (server.isNickNameTaken(nickname) == true)
    {
        sendMessage(server, client, 433, nickname + "Nickname is already in use");
        return;
    }

    client.setNickName(nickname);
    client.setHasNick(true);
}

// USER

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
        sendMessage(server, client, 462, "you may not register");
        return;
    }
    std::string username, hostname, servername, realname;
    iss >> username >> hostname >> servername;
    std::getline(iss, realname);
    if (!realname.empty() && (realname[0] == ' ' || realname[0] == ':'))
        realname.erase(0, 1);
    if (username.empty() || hostname.empty() ||
        servername.empty() || realname.empty())
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

void executeJoin(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
<<<<<<< HEAD
    std::string key;

    iss >> channelName >> key;
=======
    iss >> channelName;
>>>>>>> origin/jmehmy

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

    if (channelName[0] != '#')
    {
        sendMessage(server, client, 403, channelName + " :No such channel");
        return;
    }

    std::map<std::string, Channel*>::iterator it =
        server.getChannel(channelName);

    Channel* channel;

<<<<<<< HEAD
    // Se o canal não existir, cria-o
=======
>>>>>>> origin/jmehmy
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

<<<<<<< HEAD
        // Verificar modo +i (Invite Only)
        if (channel->isInvitedOnly() && !channel->isInvited(client))
        {
            sendMessage(server, client, 473,
                channelName + " :Cannot join channel (+i)");
            return;
        }

        // Verificar modo +k (Password)
        if (channel->isPasswordProtected()
            && key != channel->getPassword())
        {
            sendMessage(server, client, 475,
                channelName + " :Cannot join channel (+k)");
            return;
        }

        // Verificar modo +l (User Limit)
        if (channel->getUserLimit() > 0
            && channel->getMemberCount() >= channel->getUserLimit())
        {
            sendMessage(server, client, 471,
                channelName + " :Cannot join channel (+l)");
            return;
        }

        // Adicionar o cliente ao canal
        channel->addMember(client);

        // Consumir o convite, se existir
        channel->removeInvitedClient(client);
    }

    std::string message =
        ":" + client.getNickName()
=======
        channel->addMember(client);
    }

    std::string message = ":" + client.getNickName()
>>>>>>> origin/jmehmy
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

    if (!message.empty() && message[0] == ' ')
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
            {
                server.sendMessageToClient(
                    (*member)->getFd(), formattedMessage);
            }
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

        server.sendMessageToClient(
            recipient->getFd(), formattedMessage);
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

    if (!reason.empty() && reason[0] == ' ')
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
        sendMessage(server, client, 442,
            channelName + " :You're not on that channel");
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

    if (channel->isEmpty())
        server.removeChannel(channelName);
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

    if (!topic.empty() && topic[0] == ' ')
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
        sendMessage(server, client, 442,
            channelName + " :You're not on that channel");
        return;
    }

    // Consultar o tópico
    if (!changingTopic)
    {
        if (channel->getTopic().empty())
        {
            sendMessage(server, client, 331,
                channelName + " :No topic is set");
        }
        else
        {
            sendMessage(server, client, 332,
                channelName + " :" + channel->getTopic());
        }

        return;
    }

    // Verificar permissões para alterar o tópico
    if (channel->isTopicRestricted() && !channel->isOperator(client))
    {
        sendMessage(server, client, 482,
            channelName + " :You're not channel operator");
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

    if (!reason.empty() && reason[0] == ' ')
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
        sendMessage(server, client, 442,
            channelName + " :You're not on that channel");
        return;
    }

    if (!channel->isOperator(client))
    {
        sendMessage(server, client, 482,
            channelName + " :You're not channel operator");
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

<<<<<<< HEAD
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
        sendMessage(server, client, 442,
            channelName + " :You're not on that channel");
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

    sendMessage(server, client, 341,
        nickname + " " + channelName);

    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " INVITE " + nickname + " :" + channelName + "\r\n";

    server.sendMessageToClient(target->getFd(), message);
}

void executeMode(Server& server, Client& client, std::istringstream& iss)
{
    std::string channelName;
    std::string modes;

    if (!client.getIsRegistered())
    {
        sendMessage(server, client, 451, "You have not registered");
        return;
    }

    iss >> channelName;

    if (channelName.empty())
    {
        sendMessage(server, client, 461, "MODE :Not enough parameters");
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
        sendMessage(server, client, 442,
            channelName + " :You're not on that channel");
        return;
    }

    iss >> modes;

    // Consultar os modos atuais
    if (modes.empty())
    {
        sendMessage(server, client, 324,
            channelName + " " + channel->getModeStr()
            + " " + channel->getModeArguments());
        return;
    }

    if (!channel->isOperator(client))
    {
        sendMessage(server, client, 482,
            channelName + " :You're not channel operator");
        return;
    }

    bool adding = true;
    std::string argument;
    std::string appliedModes;
    std::string appliedArguments;

    for (std::string::size_type i = 0; i < modes.size(); ++i)
    {
        char mode = modes[i];

        if (mode == '+')
        {
            adding = true;
            appliedModes += '+';
            continue;
        }
        if (mode == '-')
        {
            adding = false;
            appliedModes += '-';
            continue;
        }

        if (mode == 'i')
        {
            channel->setInvitedOnly(adding);
            appliedModes += 'i';
        }
        else if (mode == 't')
        {
            channel->setTopicRestricted(adding);
            appliedModes += 't';
        }
        else if (mode == 'k')
        {
            if (adding)
            {
                if (!(iss >> argument) || argument.empty())
                {
                    sendMessage(server, client, 461,
                        "MODE :Not enough parameters");
                    return;
                }

                channel->setPassword(argument);
                channel->setPasswordProtected(true);
                appliedArguments += " " + argument;
            }
            else
            {
                channel->setPassword("");
                channel->setPasswordProtected(false);
            }

            appliedModes += 'k';
        }
        else if (mode == 'o')
        {
            if (!(iss >> argument))
            {
                sendMessage(server, client, 461,
                    "MODE :Not enough parameters");
                return;
            }

            Client* target = server.getClientByNickname(argument);

            if (target == NULL)
            {
                sendMessage(server, client, 401,
                    argument + " :No such nick");
                return;
            }

            if (!channel->isMember(*target))
            {
                sendMessage(server, client, 441,
                    argument + " " + channelName
                    + " :They aren't on that channel");
                return;
            }

            if (adding)
                channel->addOperator(*target);
            else
                channel->removeOperator(*target);

            appliedModes += 'o';
            appliedArguments += " " + argument;
        }
        else if (mode == 'l')
        {
            if (adding)
            {
                if (!(iss >> argument))
                {
                    sendMessage(server, client, 461,
                        "MODE :Not enough parameters");
                    return;
                }

                std::istringstream numberStream(argument);
                size_t limit;
                char extra;

                if (!(numberStream >> limit) ||
                    (numberStream >> extra) || limit == 0)
                {
                    sendMessage(server, client, 461,
                        "MODE :Invalid user limit");
                    return;
                }

                channel->setUserLimit(limit);
                appliedArguments += " " + argument;
            }
            else
            {
                channel->setUserLimit(0);
            }

            appliedModes += 'l';
        }
        else
        {
            sendMessage(server, client, 472,
                std::string(1, mode) + " :is unknown mode char to me");
            return;
        }
    }

    std::string message =
        ":" + client.getNickName() +
        "!" + client.getUserName() +
        "@" + client.getHostName() +
        " MODE " + channelName + " " +
        appliedModes + appliedArguments + "\r\n";

    std::vector<Client*> members = channel->getMembers();

    for (std::vector<Client*>::iterator member = members.begin();
         member != members.end(); ++member)
    {
        server.sendMessageToClient((*member)->getFd(), message);
    }
}
=======
void executePing(Server& server, Client& client, std::istringstream& iss)
{
    std::string tokenPing;
    iss >> tokenPing;

    if (tokenPing.empty())
    {
        sendMessage(server, client, 409, "No origin specified");
        return;
    }

    if (tokenPing[0] == ':')
        tokenPing.erase(0, 1);

    if (tokenPing.empty())
    {
        sendMessage(server, client, 409, "No origin specified");
        return;
    }

    std::ostringstream oss;
    oss << ":" << "ircserv" << " PONG "
        << "ircserv" << " :" << tokenPing << "\r\n";

    server.sendMessageToClient(client.getFd(), oss.str());
}

void	executeQuit(Server& server, Client& client, std::istringstream& iss)
{
	std::string quitMessage;
	std::getline(iss, quitMessage);

	while (!quitMessage.empty() && quitMessage[0] == ' ')
        quitMessage.erase(0, 1);
    if(!quitMessage.empty() && quitMessage[0] == ':')
        quitMessage.erase(0, 1);
    if(quitMessage.empty())
		quitMessage = "Client Quit";
	
	server.notifySharedChannelsQuit(client, quitMessage);
	server.handleClientDisconnection(client.getFd());
}
>>>>>>> origin/jmehmy
