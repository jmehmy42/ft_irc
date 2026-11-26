#include "Server.hpp"
#include "Client.hpp"
#include "Executor.hpp"

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
                    sendMessage(server, client, 461, "MODE :Not enough parameters");
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
                sendMessage(server, client, 461, "MODE :Not enough parameters");
                return;
            }
            Client* target = server.getClientByNickname(argument);
            if (target == NULL)
            {
                sendMessage(server, client, 401, argument + " :No such nickname");
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
        else if (mode == 'v')
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
                channel->addVoice(*target);
            else
                channel->removeVoice(*target);
            appliedModes += 'v';
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