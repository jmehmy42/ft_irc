/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kwillian <kwillian@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/01 22:14:14 by kwillian          #+#    #+#             */
/*   Updated: 2026/11/20 14:52:53 by kwillian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name),_topic(""), _clients(),
        _inviteOnly(false), _topicRestricted(false), _passwordProtected(false),
        _operatorOnly(false), _userLimit(0)
{
}

Channel::Channel(const std::string& name, Client& creator) : _name(name),_topic(""),
        _clients(), _inviteOnly(false), _topicRestricted(false), _passwordProtected(false),
        _operatorOnly(false), _userLimit(0)
{
    addMember(creator);
    addOperator(creator);
}

Channel::~Channel()
{
}

//gettters

std::string Channel::getName()const
{
    return _name;
}
const std::vector<Client*>& Channel::getClients()const
{
    return _clients;
}
size_t Channel::getMemberCount()const
{
    return _members.size();
}
std::vector<Client*> Channel::getMembers() const
{
    return _members;
}
std::vector<Client*> Channel::getOperators()const
{
    return _operators;
}

void Channel::addClient(Client* client)
{
    if(client == NULL)
        return;
    for(size_t i = 0; i < _clients.size(); i++)
    {
        if(client == _clients[i])
            return;
    }
    _clients.push_back(client);
    _members.push_back(client);
}

void Channel::removeClient(Client* client)
{
    if (client == NULL)
        return;

    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i] == client)
        {
            _clients.erase(_clients.begin() + i);
            break;
        }
    }

    for (std::vector<Client*>::iterator it = _members.begin();
         it != _members.end(); ++it)
    {
        if (*it == client)
        {
            _members.erase(it);
            break;
        }
    }

    removeOperator(*client);
    removeVoice(*client);
}

bool Channel::hasClient(Client* client)const
{
    if(client == NULL)
        return false;
    for(size_t i = 0; i < _clients.size(); i++)
    {
        if(client == _clients[i])
            return true;
    }
    return false;
}

bool Channel::isEmpty()const
{
    if(_clients.size() == 0)
        return true;
    return false;
}

void Channel::addMember(Client& client)
{
    addClient(&client);
}
void Channel::removeMember(Client& client)
{
    removeClient(&client);
}
bool Channel::isMember(const Client& client) const
{
    for(size_t i = 0; i < _members.size(); ++i)
    {
        if(_members[i]->getFd() == client.getFd())
            return true;
    }
    return false;
}

void Channel::addOperator(Client& client)
{
    _operators.push_back(&client);
}
void Channel::removeOperator(Client& client)
{
    for(std::vector<Client*>::iterator it= _operators.begin(); it!= _operators.end(); ++it)
    {
        if(*it == &client)
        {
            _operators.erase(it);
            return;
        }
    }
}

bool Channel::isOperator(const Client& client) const
{
    for(size_t i = 0; i < _operators.size(); ++i)
    {
        if(_operators[i]->getFd() == client.getFd())
            return true;
    }
    return false;
}

void Channel::addInvitedClient(Client& client)
{
    _invited.push_back(&client);
}
void Channel::removeInvitedClient(Client& client)
{
    for(std::vector<Client*>::iterator it = _invited.begin(); it != _invited.end(); ++it)
    {
        if(*it == &client)
        {
            _invited.erase(it);
            return;
        }
    }
}
bool Channel::isInvited(const Client& client) const
{
    for(size_t i = 0; i < _invited.size(); ++i)
    {
        if(_invited[i]->getFd() == client.getFd())
            return true;
    }
    return false;
}

void Channel::broadcastToMembers(const std::string& message, int excludeFd)
{
    for(size_t i = 0; i < _members.size(); ++i)
    {
        if(_members[i]->getFd() != excludeFd)
        {
            int bytesent = send(_members[i]->getFd(), message.c_str(), message.length(), 0);
            if(bytesent == -1)
                std::cerr << "send failed to client "<< _members[i]->getFd() << std::endl;
        }
    }
}

const std::string& Channel::getTopic() const
{
    return _topic;
}

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}
bool Channel::isInvitedOnly()const
{
    return _inviteOnly;
}
void Channel::setInvitedOnly(bool status)
{
    _inviteOnly = status;
}
bool Channel::isTopicRestricted()const
{
    return _topicRestricted;
}
void Channel::setTopicRestricted(bool status)
{
    _topicRestricted = status;
}
bool Channel::isPasswordProtected()const
{
    return _passwordProtected;
}

void Channel::setPasswordProtected(bool status)
{
    _passwordProtected = status;
}
const std::string& Channel::getPassword()const
{
    return _password;
}

void Channel::setPassword(const std::string& password)
{
    _password = password;
}
bool Channel::isOperatorOnly()const
{
    return _operatorOnly;
}

void Channel::setOperatorOnly(bool status)
{
    _operatorOnly = status;
}
size_t Channel::getUserLimit()const
{
    return _userLimit;
}

void Channel::setUserLimit(size_t limit)
{
    _userLimit = limit;
}

std::string Channel::getModeStr()const
{
    std::string modes = "+";
    if(isInvitedOnly())
        modes += "i";
    if(isTopicRestricted())
        modes += "t";
    if(isPasswordProtected())
        modes += "k";
    if(isOperatorOnly())
        modes += "o";
    if(getUserLimit() > 0)
        modes += "l";
    return modes;
}

std::string Channel::getModeArguments()const
{
    std::string args;
    if(isPasswordProtected())
        args += " " + getPassword();
    if(getUserLimit() > 0)
    {
        std::ostringstream oss;
        oss << getUserLimit();
        args += " " + oss.str();
    }
    return args;
}

void Channel::addVoice(Client& client)
{
    if (!isVoice(client))
        _voices.push_back(&client);
}

void Channel::removeVoice(Client& client)
{
    for (std::vector<Client*>::iterator it = _voices.begin();
         it != _voices.end(); ++it)
    {
        if ((*it)->getFd() == client.getFd())
        {
            _voices.erase(it);
            return;
        }
    }
}

bool Channel::isVoice(const Client& client) const
{
    for (size_t i = 0; i < _voices.size(); ++i)
    {
        if (_voices[i]->getFd() == client.getFd())
            return true;
    }
    return false;
}
