#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <cstddef>
#include <iostream>
#include <exception>

#define LINE_MAX 512
#define BUFFER_MAX 1024

class Client
{
    private:
        int         _fd;
        std::string _passWord;
        std::string _hostName;
        std::string _nickName;
        std::string _userName;
        std::string _realName;
        std::string _buffer;

        bool        _hasPass;
        bool        _hasNick;
        bool        _hasUser;
        bool        _isRegistered;

    public:
        Client();
        Client(int fd, const std::string& host);
        ~Client();

        //getters
        int             getFd() const;
        std::string     getPassWord() const;
        std::string     getNickName() const;
        std::string     getUserName() const;
        std::string     getHostName() const;
        std::string     getRealName() const;
        std::string     getBuffer() const;
        bool            getHasPass() const;
        bool            getHasNick() const;
        bool            getHasUser() const;
        bool            getIsRegistered() const;

        //setters
        void            setFd(int fd);
        void            setPassWord(const std::string& passWord);
        void            setNickName(const std::string& nickName);
        void            setUserName(const std::string& userName);
        void            setHostName(const std::string& hostName);
        void            setRealName(const std::string& realName);
        void            setBuffer(const std::string& buffer);
        void            setHasPass(bool status);
        void            setHasNick(bool status);
        void            setHasUser(bool status);
        void            setIsRegistered(bool status);
        void            appendToBuffer(const std::string& data);
        bool            hasCompleteCommand()const;
        std::string     extractNextCommand();
};

#endif
/*
Socket file descriptor. Server uses it to identify and communicate with this client.
int _fd;

Password entered by the client using the PASS command.
std::string _passWord;

Client's IP address or hostname, used in IRC message prefixes.
std::string _hostName;

Client's IRC nickname, set using the NICK command.
std::string _nickName;

Username provided by the client using the USER command.
std::string _userName;

Real name provided by the client using the USER command.
std::string _realName;

Stores incoming data until we receive a complete IRC command ending with \r\n.
std::string _buffer;

Becomes true after the client sends a valid PASS command.
bool _hasPass;

Becomes true after the client sets a nickname using NICK.
bool _hasNick;

Becomes true after the client sends the USER command.
bool _hasUser;

Becomes true when the client has completed the full registration process.
bool _isRegistered;

Creates an empty client object.
Client();

Creates a client with its socket descriptor and hostname/IP address.
Client(int fd, const std::string& host);

Cleans up the client object when it is destroyed.
~Client();

Adds newly received data to the buffer until a full command is available.
void appendToBuffer(const std::string& data);

Checks whether the buffer contains a complete IRC command.
bool hasCompleteCommand() const;

Removes and returns the next complete command from the buffer.
std::string extractNextCommand();
*/