
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

        //memebers
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

        //modes
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

        void                        broadcastToMembers(const std::string& message, int excludeFd);
};

#endif

/*
The Channel class manages one IRC channel.

It stores:
- Channel name
- Connected clients / members
- Channel operators
- Invited clients
- Channel topic
- Channel password
- Channel modes (+i, +t, +k, +l, +o)

The command handlers (JOIN, PART, KICK, INVITE, TOPIC, MODE, PRIVMSG)
use these functions to manage the channel.

Server is responsible for creating, finding and deleting channels.
Channel is responsible for managing the information inside each channel.


 CONSTRUCTORS

Channel(std::string name)
    Creates a new channel with the given name.

    Use in:
    JOIN - when a channel does not exist yet.

Channel(const std::string& name, Client& creator)
    Creates a new channel and automatically adds the creator as:
    - A channel member
    - A channel operator

    Use in:
    JOIN - when creating a new channel.

------------------ BASIC CHANNEL INFORMATION-----------------

getName()
    Returns the channel name, for example "#test".

    Use in:
    JOIN, PART, KICK, INVITE, TOPIC, MODE, PRIVMSG.

getClients()
    Returns the list of clients stored in the channel (_clients).
    The returned list cannot be modified through this function.

    Use in:
    When a command needs to loop through the channel's clients.

addClient(Client* client)
    Adds a client pointer to the channel.
    Does nothing if the pointer is NULL or the client is already present.
    Also adds the client to the _members list.

    Use in:
    Normally through addMember().

removeClient(Client* client)
    Removes a client pointer from _clients and _members.

    Use in:
    Normally through removeMember().

hasClient(Client* client)
    Checks whether a client pointer is already present in the channel.

    Returns true if the client exists, otherwise false.

    Use in:
    JOIN - to check whether the client is already in the channel.
    INVITE - to check whether the target is already a member.

isEmpty()
    Checks whether the channel has no clients.

    Returns true if the channel is empty.

    Use in:
    PART, KICK, QUIT - after removing a member.
    If the channel is empty, Server can delete it.



-----------------CHANNEL TOPIC-------------------------

getTopic()
    Returns the current channel topic.

    Use in:
    TOPIC - when a client asks to see the current topic.

setTopic(const std::string& topic)
    Changes the channel topic.

    Use in:
    TOPIC - when a client wants to set a new topic.



-------------------CHANNEL MEMBERS------------------------


addMember(Client& client)
    Adds a client to the channel.
    Internally calls addClient().

    Use in:
    JOIN - after all checks are successful.

removeMember(Client& client)
    Removes a client from the channel.
    Internally calls removeClient().

    Use in:
    PART - when a client leaves.
    KICK - when a client is kicked.
    QUIT - when a client disconnects.

isMember(const Client& client)
    Checks whether a client is a member of this channel.

    Returns true if the client is a member, otherwise false.

    Use in:
    PART - check whether the sender is in the channel.
    KICK - check whether sender and target are members.
    INVITE - check whether sender is a member.
    PRIVMSG - check whether sender can send to the channel.
    TOPIC - check membership when required.



-------------------CHANNEL OPERATORS-------------------------


addOperator(Client& client)
    Gives a client channel-operator privileges.

    Use in:
    MODE +o - to give operator status.
    Channel creation - the creator is automatically made an operator
    when using the constructor with Client& creator.

removeOperator(Client& client)
    Removes a client's channel-operator privileges.

    Use in:
    MODE -o - to remove operator status.
    PART, KICK, QUIT - when cleaning up a departing operator.

isOperator(const Client& client)
    Checks whether a client is a channel operator.

    Returns true if the client is an operator, otherwise false.

    Use in:
    KICK - check whether sender has permission.
    MODE - check whether sender can change channel modes.
    TOPIC - check permission when +t is enabled.
    INVITE - check permission when required by channel rules.



-------------------CHANNEL INVITATIONS-------------------------------


addInvitedClient(Client& client)
    Adds a client to the channel's invitation list.

    Use in:
    INVITE - after checking the sender's permission.

removeInvitedClient(Client& client)
    Removes a client from the invitation list.

    Use in:
    JOIN - after an invited client successfully joins.
    QUIT / cleanup - when removing invitation information if necessary.

isInvited(const Client& client)
    Checks whether a client has been invited to this channel.

    Returns true if the client is invited, otherwise false.

    Use in:
    JOIN - when the channel has invite-only mode (+i).



-------------------MEMBER AND OPERATOR INFORMATION---------------------------


getMemberCount()
    Returns the number of members currently in the channel.

    Use in:
    JOIN - to check whether the channel has reached its user limit (+l).

getMembers()
    Returns a copy of the channel's member list.

    Use in:
    JOIN - to build the channel's member names list.
    PRIVMSG - when looping through members.
    Other commands that need to inspect channel members.

getOperators()
    Returns a copy of the channel's operator list.

    Use in:
    Commands that need to inspect or display channel operators.
    For example, NAMES or MODE.



-------------------CHANNEL MODES----------------------------
MODE +i

isInviteOnly()
    Checks whether invite-only mode is enabled.

    Use in:
    JOIN - check whether the client needs an invitation.

setInviteOnly(bool status)
    Enables or disables invite-only mode.

    Use in:
    MODE +i - set true.
    MODE -i - set false.


----------------------------- MODE +t ----------------------------------------

isTopicRestricted()
    Checks whether only channel operators can change the topic.

    Use in:
    TOPIC - check permission before changing the topic.

setTopicRestricted(bool status)
    Enables or disables topic restriction.

    Use in:
    MODE +t - set true.
    MODE -t - set false.


----------------------------- MODE +k ----------------------------------------

isPasswordProtected()
    Checks whether the channel requires a password.

    Use in:
    JOIN - check whether a password is required.

getPassword()
    Returns the channel password.

    Use in:
    JOIN - compare the password provided by the client.

setPassword(const std::string& password)
    Stores the channel password.

    Use in:
    MODE +k - set the channel password.

setPasswordProtected(bool status)
    Enables or disables password protection.

    Use in:
    MODE +k - set true.
    MODE -k - set false.


----------------------------- MODE +o ----------------------------------------

isOperatorOnly()
    Returns the value of the _operatorOnly variable.

setOperatorOnly(bool status)
    Changes the value of the _operatorOnly variable.

    Note:
    These two functions manage the separate _operatorOnly boolean.
    Individual channel operator privileges are managed by:
    addOperator(), removeOperator() and isOperator().

    Check the intended meaning of _operatorOnly before using it for MODE +o.


----------------------------- MODE +l ----------------------------------------

getUserLimit()
    Returns the maximum number of members allowed in the channel.

    Use in:
    JOIN - check whether the channel is full.

setUserLimit(size_t limit)
    Sets the maximum number of members allowed.

    Use in:
    MODE +l - set the limit.
    MODE -l - remove the limit according to the implementation.


-------------------------- MODE INFORMATION ---------------------------------

getModesStr()
    Returns a string containing the currently enabled modes.

    Example:
    +itkl

    Use in:
    MODE - when a client asks to see the current channel modes.

getModeParamsStr()
    Returns the parameters associated with active modes,
    such as the channel password and user limit.

    Use in:
    MODE - when replying with the current channel modes.



------------------- COMMUNICATION---------------------------------


broadcastToMembers(const std::string& message, int excludeFd)
    Sends a message to the channel members.

    The client whose file descriptor matches excludeFd is skipped.

    Use in:
    JOIN - notify existing members when someone joins.
    PART - notify members when someone leaves.
    KICK - notify members when someone is kicked.
    TOPIC - notify members when the topic changes.
    MODE - notify members when a mode changes.
    PRIVMSG - send a message to channel members.

    Note:
    This function currently uses send() directly.
    Check with the Server implementation before relying on it,
    especially because the server uses non-blocking sockets.



-------------------COMMANDS AND THEIR FUNCTIONS-----------------------------


JOIN:
    addMember()
    isMember()
    hasClient()
    isInviteOnly()
    isInvited()
    removeInvitedClient()
    isPasswordProtected()
    getPassword()
    getMemberCount()
    getUserLimit()
    broadcastToMembers()

PART:
    isMember()
    removeMember()
    removeOperator()
    broadcastToMembers()
    isEmpty()

KICK:
    isMember()
    isOperator()
    removeMember()
    removeOperator()
    broadcastToMembers()
    isEmpty()

INVITE:
    isMember()
    isOperator()
    hasClient()
    addInvitedClient()
    isInviteOnly()

TOPIC:
    getTopic()
    setTopic()
    isTopicRestricted()
    isOperator()
    isMember()
    broadcastToMembers()

MODE:
    isOperator()
    setInviteOnly()
    setTopicRestricted()
    setPassword()
    setPasswordProtected()
    addOperator()
    removeOperator()
    setUserLimit()
    getModesStr()
    getModeParamsStr()
    broadcastToMembers()

PRIVMSG:
    isMember()
    getMembers()
    getClients()
    broadcastToMembers()

QUIT / DISCONNECTION:
    isMember()
    removeMember()
    removeOperator()
    removeInvitedClient()
    broadcastToMembers()
    isEmpty()



-------------------IMPORTANT NOTES--------------------------


1. Commands should use the public Channel functions instead of directly
   changing _members, _operators, _invited or the mode variables.

2. Server manages the channel map: finding, creating and deleting channels.
   Channel manages the information inside one channel.

3. removeMember() does not remove operator status or invitation status.
   Clean those lists separately when needed.

4. The constructor Channel(name, creator) already adds the creator as a
   member and an operator. Do not add the creator a second time.

5. The _operatorOnly variable is separate from the _operators list.
   Do not confuse it with the normal IRC +o operator privilege.

*/
