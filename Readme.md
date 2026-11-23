*This project has been created as part of the 42 curriculum by jmehmy kwillian.*

# ft_irc

## Description

A lightweight IRC (Internet Relay Chat) server written in C++98, developed as part of the 42 School curriculum.

The goal of this project is to build an IRC server capable of accepting multiple simultaneous clients, handling user registration, managing channels, and allowing users to communicate through private and channel messages.

The server follows the IRC communication model and allows users to connect through IRC clients such as `nc` (Netcat).

IRC (Internet Relay Chat) is a communication protocol that allows users to exchange messages in real time.

In this project, the objective is to implement an IRC server from scratch using C++98.

The server is responsible for:

- Accepting incoming client connections.
- Managing multiple connected clients.
- Authenticating users through a server password.
- Handling nicknames and user registration.
- Creating and managing channels.
- Managing channel operators and permissions.
- Routing private and channel messages.
- Processing IRC commands and sending appropriate replies.

The project was developed following the restrictions and requirements of the 42 ft_irc subject.

## Features

### Server

- TCP socket communication.
- Multiple simultaneous client connections.
- Client registration and authentication.
- Non-blocking or multiplexed client communication (describe the actual mechanism used in your implementation).
- Individual client management.
- Command parsing and execution.

### User Management

- Password authentication (`PASS`).
- Nickname registration (`NICK`).
- User information registration (`USER`).
- Nickname uniqueness validation.
- Client disconnection (`QUIT`).

### Channel Management

- Channel creation and joining.
- Channel member management.
- Channel operators.
- Voice permissions.
- Channel topics.
- Invitations.
- Kicking users from channels.
- Channel mode management.

### Messaging

- Private messages.
- Channel messages.
- Server replies and error numerics.
- PING/PONG handling.

---

## Technologies

| Technology | Purpose |
|---|---|
| C++98 | Main programming language |
| BSD Sockets | Network communication |
| TCP/IP | Client-server communication |
| Makefile | Compilation and build management |
| Netcat (`nc`) | Manual testing |

---

## Project Structure

The project is organized into different classes, each responsible for a specific part of the server.

```text
ft_irc/
├── Makefile
├── Readme.md
├── includes/
│   ├── Channel.hpp
│   ├── Client.hpp
│   ├── Commands.hpp
│   ├── Executor.hpp
│   └── Server.hpp
└── src/
    ├── Channel.cpp
    ├── Client.cpp
    ├── Executor.cpp
    ├── main.cpp
    ├── commands/
    │   ├── ChannelCommands.cpp
    │   ├── Connection.cpp
    │   ├── Messaging.cpp
    │   ├── Mode.cpp
    │   └── Registration.cpp
    └── server/
        ├── ChannelManager.cpp
        ├── ClientManager.cpp
        ├── Connection.cpp
        ├── Messaging.cpp
        └── Server.cpp
```

### Main Classes

**Server**

Responsible for creating the socket, listening for incoming connections, accepting clients, and managing communication.

**Client**

Stores information about each connected user, including:

- Socket file descriptor.
- Nickname.
- Username.
- Real name.
- Registration status.
- Supported capabilities.
- Input buffer.

**Channel**

Manages channel information, including:

- Channel name.
- Topic.
- Members.
- Operators.
- Voice permissions.
- Invitations.
- Channel modes.

**Executor**

Responsible for parsing incoming commands and dispatching them to the appropriate command handlers.

---

## Instructions

### Compilation

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/ft_irc.git
cd ft_irc
```

Compile the project:

```bash
make
```

The compilation should generate the server executable:

```text
ircserv
```

Additional Makefile commands:

```bash
make clean
make fclean
make re
```

---

### Running the Server

The server requires two arguments:

1. Port number.
2. Server password.

Usage:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6668 password123
```

Expected output:

```text
Server listening on port 6668
```

The server is now ready to accept client connections.

---

### Connecting to the Server

You can use Netcat (`nc`) to connect to the server.

Open a new terminal:

```bash
nc -C 127.0.0.1 6668
```

The `-C` option makes Netcat send CRLF line endings, which are used by IRC.

You can open multiple terminals to simulate different users connected to the same server.

---

### User Registration

Before using the server, a client must complete the registration process.

Send the following commands:

```irc
PASS password123
NICK Alice
USER alice 0 * :Alice Test
```

Explanation:

| Command | Description |
|---|---|
| PASS | Provides the server password |
| NICK | Sets the user's nickname |
| USER | Provides the user's username and real name |

Once registration is complete, the client can start using the IRC commands.

---

### Available Commands

The following commands are implemented in this project.

| Command | Description | Example |
|---|---|---|
| PASS | Authenticates the client | `PASS password123` |
| NICK | Sets or changes a nickname | `NICK Alice` |
| USER | Registers user information | `USER alice 0 * :Alice Test` |
| JOIN | Joins or creates a channel | `JOIN #42` |
| PRIVMSG | Sends a private or channel message | `PRIVMSG #42 :Hello!` |
| PART | Leaves a channel | `PART #42` |
| TOPIC | Sets or retrieves a channel topic | `TOPIC #42 :Welcome!` |
| KICK | Removes a user from a channel | `KICK #42 Bob :Reason` |
| INVITE | Invites a user to a channel | `INVITE Bob #42` |
| MODE | Changes channel modes | `MODE #42 +i` |
| NAMES | Lists channel members | `NAMES #42` |
| CAP | Negotiates client capabilities | `CAP LS` |
| PING | Checks server connectivity | `PING :hello` |
| PONG | Replies to a PING | `PONG :hello` |
| QUIT | Disconnects from the server | `QUIT :Goodbye` |

### Example: Channel Communication

Client Alice:

```irc
JOIN #42
```

Client Bob:

```irc
JOIN #42
```

Alice sends a message:

```irc
PRIVMSG #42 :Hello Bob!
```

Bob receives the message from Alice.

---

### Channel Modes

The server supports the following channel modes:

| Mode | Description | Example |
|---|---|---|
| `i` | Invite-only channel | `MODE #42 +i` |
| `t` | Restricts topic changes to channel operators | `MODE #42 +t` |
| `k` | Sets a channel password | `MODE #42 +k secret` |
| `o` | Grants or removes operator privileges | `MODE #42 +o Bob` |
| `v` | Grants or removes voice privileges | `MODE #42 +v Bob` |
| `l` | Sets a channel member limit | `MODE #42 +l 10` |

To remove a mode, use `-` instead of `+`.

Example:

```irc
MODE #42 -i
MODE #42 -k secret
MODE #42 -v Bob
```

### Channel Roles

**Operator (`@`)**

Channel operators have administrative privileges, such as managing channel modes and removing users.

**Voice (`+`)**

Users with voice have a special channel status. This status is relevant to moderated channels.

---

### Capabilities

The server supports IRC capability negotiation through the `CAP` command.

Currently implemented capability:

### multi-prefix

Allows clients to receive all applicable channel membership prefixes.

For example, a user who is both an operator and has voice can appear as:

```text
@+Alice
```

Without `multi-prefix`, only the highest-priority prefix is displayed.

Example negotiation:

```irc
CAP LS
CAP REQ :multi-prefix
CAP LIST
CAP END
```

---

### Testing

The server can be tested manually using multiple Netcat connections.

Open three terminals and connect three users:

```bash
nc -C 127.0.0.1 6668
```

Register each user with a different nickname.

Suggested test scenarios:

- User registration and password validation.
- Multiple simultaneous connections.
- Joining and leaving channels.
- Private messages.
- Channel messages.
- Topic management.
- Operator permissions.
- Voice permissions.
- Invitations and kicking users.
- Channel modes.
- Capability negotiation.
- PING/PONG.
- Client disconnection.

Testing should include both valid commands and invalid input, such as missing parameters, nonexistent nicknames, and insufficient permissions.

---

## How It Works

The server follows a client-server architecture.

1. The server creates a TCP socket.
2. It binds the socket to the requested port.
3. It starts listening for incoming connections.
4. When a client connects, the server accepts the connection.
5. Incoming data is read and processed.
6. Commands are parsed and dispatched by the Executor.
7. The appropriate class performs the requested operation.
8. The server sends the corresponding reply to the client or channel members.

This architecture separates network communication, client management, channel management, and command execution.

---

## AI USED

- To explain and clarify the ports, protocols as TCP, netcat and all the other concepts.
- Helped a lot to find and fix issues explaining why should be in another way and giving some ideias to improve the code eficiency.

--- 

# Resources

- https://medium.com/@mohcin.ghalmi/irc-server-internet-relay-chat-bd08e4f469f8
- https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9
- https://www.youtube.com/watch?v=3e3aZ8ol4hI

---

## Possible Improvements

Some possible future improvements include:

- Support for additional IRC capabilities.
- More complete IRC protocol compatibility.
- Improved error handling.
- Additional automated tests.
- Better logging and debugging tools.
