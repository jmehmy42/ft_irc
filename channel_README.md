
# Channel.hpp / Channel.cpp — How to use it

The idea is simple:

The  Channel  class for managing everything related to a channel.

So when making commands like  JOIN ,  KICK ,  MODE ,  INVITE ,  TOPIC ,  PART , etc., **don't need to manage the channel data yourself**.

Just get the  Channel*  from the Server and use the functions from  Channel.hpp .

Think of it like this:

 
Server
  |
  |-- gives you Channel*
  |
  v
Channel
  |
  |-- members
  |-- operators
  |-- invited users
  |-- topic
  |-- password
  |-- modes
 

Your command is responsible for deciding **what should happen**.

The  Channel  class is responsible for **storing and changing the channel information**.

---

## 1. Basic channel functions

###  getName() 

Returns the channel name.

For example:

 cpp
channel->getName();
 

Use it when you need the channel name while creating replies/messages.

You can use it in basically any channel command:

- JOIN
- PART
- KICK
- INVITE
- TOPIC
- MODE
- PRIVMSG

---

###  getClients() 

Gives you the clients stored in the channel.

Use this when you need to look through the clients.

For example, in  PRIVMSG , you may need to send the message to the people inside the channel.

---

###  isEmpty() 

Checks if there is nobody left in the channel.

Mostly useful after:

- PART
- KICK
- QUIT

For example:

 cpp
channel->removeMember(client);

if (channel->isEmpty())
{
    // remove channel from Server
}
 

The Server should handle removing the channel itself.

---

# 2. Members

These are the functions you will probably use the most.

###  addMember(Client& client) 

Adds a client to the channel.

The main place to use this is:

**JOIN**

After all the JOIN checks pass:

 cpp
channel->addMember(client);
 

So basically:

 
JOIN
 |
 |-- check password
 |-- check invite
 |-- check limit
 |
 |-- everything OK
 |
 v
addMember()
 

---

###  removeMember(Client& client) 

Removes a client from the channel.

Use it when somebody leaves the channel.

Main places:

- PART
- KICK
- QUIT

Example:

 cpp
channel->removeMember(client);
 

---

###  isMember(const Client& client) 

Checks if a client is inside the channel.

This is very important for commands.

For example, in  PART :

 cpp
if (!channel->isMember(client))
{
    // error
}
 

In  KICK , you can use it to check:

 
Is the person doing KICK inside the channel?
Is the target inside the channel?
 

In  PRIVMSG :

 
Is the sender actually inside this channel?
 

So you will use  isMember()  in many commands.

---

###  hasClient(Client* client) 

Also checks whether a client exists in the channel.

Use this when you specifically have a  Client* .

For example, in  JOIN :

 
Client tries to JOIN
       |
       v
Is he already in the channel?
       |
       v
hasClient()
 

If already there, don't add him again.

---

# 3. Operators

Operators are the people who have channel privileges.

###  addOperator(Client& client) 

Makes a client an operator.

Main place:

**MODE +o**

Example:

 
MODE #test +o bob
 

Then:

 cpp
channel->addOperator(bob);
 

The person who creates the channel is also made operator when using the constructor that takes the creator.

---

###  removeOperator(Client& client) 

Removes operator privilege.

Main place:

**MODE -o**

Example:

 
MODE #test -o bob
 

Then:

 cpp
channel->removeOperator(bob);
 

Also remember to clean operator status when someone leaves or gets kicked.

---

###  isOperator(const Client& client) 

Checks if someone is a channel operator.

This is the function you use before commands that require operator permission.

For example:

### KICK

 
Is sender an operator?
    |
   YES -> allow KICK
    |
    NO -> error
 

### INVITE

Depending on your IRC rules, check operator permission when the channel is invite-only.

### MODE

Normally only an operator should be able to change channel modes.

### TOPIC

If  +t  is active, only an operator can change the topic.

So:

 cpp
if (!channel->isOperator(client))
{
    // no permission
}
 

---

# 4. Invitations

These are mainly for  INVITE  and  JOIN .

###  addInvitedClient(Client& client) 

Adds somebody to the invitation list.

Use this in:

**INVITE**

Example:

 
INVITE bob #test
 

After checking the permission:

 cpp
channel->addInvitedClient(bob);
 

---

###  isInvited(const Client& client) 

Checks whether the client was invited.

Main place:

**JOIN**

Especially when channel has  +i .

Example:

 
JOIN #private
       |
       v
Is channel invite-only?
       |
      YES
       |
       v
Was this client invited?
       |
   YES -> allow
   NO  -> error
 

---

###  removeInvitedClient(Client& client) 

Removes the client from the invitation list.

Normally after the client successfully joins:

 cpp
channel->removeInvitedClient(client);
 

So an invitation doesn't stay there forever after it has been used.

---

# 5. Channel topic

These functions are for  TOPIC .

###  getTopic() 

Gets the current topic.

Use it when the user does:

 
TOPIC #test
 

without giving a new topic.

Then you return the current topic.

---

###  setTopic(const std::string& topic) 

Changes the channel topic.

Use it when:

 
TOPIC #test Hello everyone
 

is received.

Before calling  setTopic() , check the permissions.

For example, if  +t  is enabled:

 cpp
if (channel->isTopicRestricted() &&
    !channel->isOperator(client))
{
    // error
}
 

Then:

 cpp
channel->setTopic(topic);
 

After changing it, broadcast the TOPIC message.

---

# 6. Channel modes

Most of your  MODE  command will work directly with these functions.

The main idea is:

 
MODE command
     |
     |-- check operator
     |
     |-- read +i / -i
     |-- read +t / -t
     |-- read +k / -k
     |-- read +o / -o
     |-- read +l / -l
     |
     v
change Channel
 

---

##  isInviteOnly() 

Checks if  +i  is active.

Main place:

**JOIN**

 cpp
if (channel->isInviteOnly())
{
    // client needs invitation
}
 

Also useful in  INVITE .

---

##  setInviteOnly(bool) 

Turns  +i  on or off.

Used by:

**MODE**

 
MODE #test +i
 

 cpp
channel->setInviteOnly(true);
 

And:

 
MODE #test -i
 

 cpp
channel->setInviteOnly(false);
 

---

##  isTopicRestricted() 

Checks whether  +t  is active.

Main place:

**TOPIC**

If it is active, only operators can change the topic.

---

##  setTopicRestricted(bool) 

Used by:

**MODE +t / -t**

 cpp
channel->setTopicRestricted(true);
 

or:

 cpp
channel->setTopicRestricted(false);
 

---

# 7. Password / +k

###  isPasswordProtected() 

Checks if the channel has password protection.

Main place:

**JOIN**

Example:

 
JOIN #test password123
 

You check:

 cpp
if (channel->isPasswordProtected())
{
    // check supplied password
}
 

---

###  getPassword() 

Gets the channel's password.

Use this in  JOIN  to compare what the user supplied.

Example:

 cpp
if (password != channel->getPassword())
{
    // wrong password
}
 

---

###  setPassword(const std::string& password) 

Sets the channel password.

Main place:

**MODE +k**

Example:

 
MODE #test +k secret
 

Then:

 cpp
channel->setPassword("secret");
 

---

###  setPasswordProtected(bool) 

Turns password protection on/off.

For:

 
MODE #test +k secret
 

you enable it:

 cpp
channel->setPasswordProtected(true);
 

For:

 
MODE #test -k
 

disable it:

 cpp
channel->setPasswordProtected(false);
 

---

# 8. User limit / +l

###  getMemberCount() 

Returns how many members are currently inside.

Main place:

**JOIN**

You use it to check if the channel is full.

---

###  getUserLimit() 

Returns the maximum number of users.

Also mainly used in:

**JOIN**

Example:

 
limit = 10
members = 10
 

Then another JOIN should fail.

---

###  setUserLimit(size_t limit) 

Sets the maximum number of users.

Main place:

**MODE +l**

Example:

 
MODE #test +l 10
 

 cpp
channel->setUserLimit(10);
 

For  -l , you can remove the limit by setting it appropriately according to the existing implementation.

---

# 9. Getting modes

###  getModesStr() 

Returns the current channel modes.

Main place:

**MODE**

When someone asks:

 
MODE #test
 

you need to show the current modes.

So:

 cpp
std::string modes = channel->getModesStr();
 

---

###  getModeParamsStr() 

Gets the parameters related to the modes.

For example, password or user-limit parameters.

Also mainly used when replying to:

 
MODE #test
 

---

# 10. Sending something to everyone in the channel

###  broadcastToMembers(message, excludeFd) 

This sends a message to the channel members.

This is useful whenever something happens in the channel and everyone needs to know.

For example:

### JOIN

Bob joins:

 
Bob JOIN #test
 

Notify the other people in  #test .

---

### PART

Bob leaves:

 
Bob PART #test
 

Notify the channel.

---

### KICK

Operator kicks Bob:

 
Alice KICK #test Bob
 

Notify the channel.

---

### TOPIC

Topic changes:

 
TOPIC #test Hello
 

Notify everyone.

---

### MODE

Mode changes:

 
MODE #test +i
 

Notify everyone.

---

### PRIVMSG

Someone sends:

 
PRIVMSG #test :hello
 

Send the message to the members.

---

# 11. How the commands should think

Don't think:

> "Which Channel function should I randomly call?"

Instead think:

> "What am I trying to do to the channel?"

Then use the matching function.

### JOIN

 
Need to check:
    already member?
    invite only?
    invited?
    password?
    full?

Then:
    addMember()

Then:
    broadcastToMembers()
 

---

### PART

 
Is member?
    |
    v
removeMember()
    |
    v
removeOperator()
    |
    v
broadcastToMembers()
    |
    v
isEmpty()
 

---

### KICK

 
Is sender member?
Is sender operator?
Is target member?

       |
       v

removeMember(target)
removeOperator(target)

       |
       v

broadcastToMembers()
 

---

### INVITE

 
Is sender allowed?
Is target already member?

       |
       v

addInvitedClient(target)
 

---

### TOPIC

 
Get topic?
    |
    v
getTopic()

OR

Change topic?
    |
    v
check isTopicRestricted()
check isOperator()
    |
    v
setTopic()
    |
    v
broadcastToMembers()
 

---

### MODE

 
Check isOperator()

       |
       +---- +i -> setInviteOnly()
       |
       +---- -i -> setInviteOnly()
       |
       +---- +t -> setTopicRestricted()
       |
       +---- -t -> setTopicRestricted()
       |
       +---- +k -> setPassword()
       |
       +---- -k -> setPasswordProtected()
       |
       +---- +o -> addOperator()
       |
       +---- -o -> removeOperator()
       |
       +---- +l -> setUserLimit()

       |
       v

broadcastToMembers()
 

---

# 12. Very short version for you

If you are writing a command, these are the important ones to remember:

 
JOIN
    -> isMember()
    -> isInviteOnly()
    -> isInvited()
    -> isPasswordProtected()
    -> getPassword()
    -> getMemberCount()
    -> getUserLimit()
    -> addMember()
    -> removeInvitedClient()
    -> broadcastToMembers()

PART
    -> isMember()
    -> removeMember()
    -> removeOperator()
    -> isEmpty()
    -> broadcastToMembers()

KICK
    -> isMember()
    -> isOperator()
    -> removeMember()
    -> removeOperator()
    -> broadcastToMembers()

INVITE
    -> isMember()
    -> isOperator()
    -> addInvitedClient()
    -> isInvited()

TOPIC
    -> isMember()
    -> isOperator()
    -> isTopicRestricted()
    -> getTopic()
    -> setTopic()
    -> broadcastToMembers()

MODE
    -> isOperator()
    -> setInviteOnly()
    -> setTopicRestricted()
    -> setPassword()
    -> setPasswordProtected()
    -> addOperator()
    -> removeOperator()
    -> setUserLimit()
    -> getModesStr()
    -> getModeParamsStr()
    -> broadcastToMembers()

PRIVMSG
    -> isMember()
    -> getMembers()
    -> broadcastToMembers()

QUIT
    -> removeMember()
    -> removeOperator()
    -> removeInvitedClient()
    -> isEmpty()
 

The main thing is: **you don't need to understand how  Channel.cpp  internally stores everything before writing the commands. Just use the public functions from  Channel.hpp  as the interface.**

For example, if you're writing  KICK , you shouldn't directly touch  _members  or  _operators . You ask the Channel:

 cpp
channel->isOperator(client);
channel->isMember(target);
channel->removeMember(target);
channel->removeOperator(target);
 

That's the purpose of the class.
