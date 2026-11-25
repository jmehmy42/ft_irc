#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <cstddef>
#include <iostream>
#include <exception>
#include <vector>

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
        std::string _outputBuffer;

        bool        _hasPass;
        bool        _hasNick;
        bool        _hasUser;
        bool        _isRegistered;
        std::vector<std::string> _capabilities;

    public:
        Client();
        Client(int fd, const std::string& host);
        ~Client();
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
        std::string&    getOutputBuffer();
        void addCapability(const std::string& capability);
        void removeCapability(const std::string& capability);
        bool hasCapability(const std::string& capability) const;
        const std::vector<std::string>& getCapabilities() const;
};

#endif

