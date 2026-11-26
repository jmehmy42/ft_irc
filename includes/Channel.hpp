
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <sstream>
#include "Client.hpp"

class Client;

class Channel 
{
    private:

        std::string             _name;
        std::string             _topic;
        std::vector<Client*>    _clients;
        std::vector<Client*>    _voices;
        bool                    _inviteOnly;
        bool                    _topicRestricted;
        bool                    _passwordProtected;
        bool                    _operatorOnly;
        size_t                  _userLimit;
        
        std::string             _password;
        std::vector<Client*>    _operators;
        std::vector<Client*>     _members;
        std::vector<Client*>    _invited;

    public:
        
        Channel(std::string name);
        Channel(const std::string& name, Client& creator);
        ~Channel();
        std::string                 getName() const;
        const std::vector<Client*>& getClients() const;
        void                        addClient(Client* client);
        void                        removeClient(Client* client);
        bool                        hasClient(Client* client)const;
        bool                        isEmpty()const;
        const std::string&          getTopic() const;
        void                        setTopic(const std::string& Topic);
        void                        addMember(Client& client);
        void                        removeMember(Client& client);
        bool                        isMember(const Client& client) const;
        void                        addOperator(Client& client);
        void                        removeOperator(Client& client);
        bool                        isOperator(const Client& client)const;
        void                        addInvitedClient(Client& client);
        void                        removeInvitedClient(Client& client);
        bool                        isInvited(const Client& client)const;
        size_t                      getMemberCount()const;
        std::vector<Client*>        getMembers()const;
        std::vector<Client*>        getOperators()const;
        bool                        isInvitedOnly()const;
        void                        setInvitedOnly(bool status);
        bool                        isTopicRestricted()const;
        void                        setTopicRestricted(bool status);
        bool                        isPasswordProtected()const;
        void                        setPasswordProtected(bool status);
        const std::string&          getPassword()const;
        void                        setPassword(const std::string& password);
        bool                        isOperatorOnly()const;
        void                        setOperatorOnly(bool status);
        size_t                      getUserLimit()const;
        void                        setUserLimit(size_t limit);
        std::string                 getModeStr()const;
        void addVoice(Client& client);
        void removeVoice(Client& client);
        bool isVoice(const Client& client) const;
        std::string                 getModeArguments()const;
        void                        assignNewOperator();
        void                        broadcastToMembers(const std::string& message, int excludeFd);
};

#endif

