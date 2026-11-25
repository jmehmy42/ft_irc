#include "Client.hpp"

Client::Client() : _fd(-1), _passWord(""), _hostName(""), _nickName(""),
                    _userName(""), _realName(""), _buffer(""), _hasPass(false), 
                    _hasNick(false), _hasUser(false), _isRegistered(false), _capabilities()
{
}

Client::Client(int fd, const std::string& host) 
                    : _fd(fd), _passWord(""), _hostName(host), _nickName(""),
                    _userName(""), _realName(""), _buffer(""), _hasPass(false), 
                    _hasNick(false), _hasUser(false), _isRegistered(false), _capabilities()
{
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

std::string Client::getPassWord() const
{
    return _passWord;
}

std::string Client::getNickName() const
{
    return _nickName;
}

std::string Client::getUserName() const
{
    return _userName;
}

std::string Client::getHostName() const
{
    return _hostName;
}

std::string Client::getRealName() const
{
    return _realName;
}

std::string Client::getBuffer() const
{
    return _buffer;
}

bool Client::getHasPass() const
{
    return _hasPass;
}

bool Client::getHasNick() const
{
    return _hasNick;
}

bool Client::getHasUser() const
{
    return _hasUser;
}

bool Client::getIsRegistered() const
{
    return _isRegistered;
}

void Client::setFd(int fd)
{
    _fd = fd;
}

void Client::setPassWord(const std::string& passWord)
{
    _passWord = passWord;
}
void Client::setNickName(const std::string& nickName)
{
    _nickName = nickName;
}

void Client::setUserName(const std::string& userName)
{
    _userName = userName;
}

void Client::setHostName(const std::string& hostName)
{
    _hostName = hostName;
}

void Client::setRealName(const std::string& realName)
{
    _realName = realName;
}

void Client::setBuffer(const std::string& buffer)
{
    _buffer = buffer;
}

void Client::setHasPass(bool status)
{
    _hasPass = status;
}

void Client::setHasNick(bool status)
{
    _hasNick = status;
}

void Client::setHasUser(bool status)
{
    _hasUser =  status;
}

void Client::setIsRegistered(bool status)
{
    _isRegistered = status;
}

void Client::addCapability(const std::string& capability)
{
    if (!hasCapability(capability))
        _capabilities.push_back(capability);
}

void Client::removeCapability(const std::string& capability)
{
    for (std::vector<std::string>::iterator it = _capabilities.begin();
         it != _capabilities.end(); ++it)
    {
        if (*it == capability)
        {
            _capabilities.erase(it);
            return;
        }
    }
}

bool Client::hasCapability(const std::string& capability) const
{
    for (std::vector<std::string>::const_iterator it = _capabilities.begin();
         it != _capabilities.end(); ++it)
    {
        if (*it == capability)
            return true;
    }
    return false;
}

const std::vector<std::string>& Client::getCapabilities() const
{
    return _capabilities;
}

std::string& Client::getOutputBuffer()
{
    return _outputBuffer;
}

void Client::appendToBuffer(const std::string& data)
{
    if(data.empty())
        return;   
    if(_buffer.size() + data.size() > BUFFER_MAX)
        throw std::runtime_error("Error: command failed!");
    _buffer = _buffer + data;
    size_t found = _buffer.rfind("\r\n");
    size_t currentlineSize;
    if(found == std::string::npos)
        currentlineSize = _buffer.size();
    else
        currentlineSize = _buffer.size() - (found + 2);
    if( currentlineSize > LINE_MAX)
        throw std::runtime_error("Error: line too long");
}

bool Client::hasCompleteCommand() const
{
    if(_buffer.empty())
        return false;
    size_t found = _buffer.rfind("\r\n");
    if(found == std::string::npos)
        return false;
    return true;
}

std::string Client::extractNextCommand()
{
    if(!hasCompleteCommand())
        return "";
    while(_buffer.find("\r\n") == 0)
        _buffer.erase(0, 2);
    size_t found = _buffer.find("\r\n");
    if(found == std::string::npos)
        return "";
    std::string cmd = _buffer.substr(0, found);
    _buffer.erase(0, found + 2);
    return cmd;
}