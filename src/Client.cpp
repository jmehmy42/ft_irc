#include "Client.hpp"

Client::Client() : _fd(-1), _passWord(""), _hostName(""), _nickName(""),
                    _userName(""), _realName(""), _buffer(""), _hasPass(false), 
                    _hasNick(false), _hasUser(false), _isRegistered(false)
{
}

Client::Client(int fd, const std::string& host) 
                    : _fd(fd), _passWord(""), _hostName(host), _nickName(""),
                    _userName(""), _realName(""), _buffer(""), _hasPass(false), 
                    _hasNick(false), _hasUser(false), _isRegistered(false)
{
}

Client::~Client()
{
}
//getter

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

//setter
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

// Adds newly received data to the client's buffer.
// TCP can deliver one IRC command in multiple pieces, so we keep
// collecting data until a complete command ending with \r\n arrives.
// Also checks that the current incomplete line does not exceed LINE_MAX.

void Client::appendToBuffer(const std::string& data)
{
    //ignore empty data, nothing to add in buffer
    if(data.empty())
        return;

    //check whether adding new data would exceed the maximum buffer size
    if(_buffer.size() + data.size() > BUFFER_MAX)
        throw std::runtime_error("Error: command failed!");
    
    //add new received data at the end of the buffer
    _buffer = _buffer + data;

    //find the last occurrence of \r\n in complete command
    size_t found = _buffer.rfind("\r\n");
    //bytes after the last \r\n are the part of an incomplete command
    size_t currentlineSize;
    if(found == std::string::npos)
        currentlineSize = _buffer.size();
    else
        currentlineSize = _buffer.size() - (found + 2);
    //command cannot be longer than 1024
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
//Returns the next complete command from the buffer
//if no complete command found return empty string

std::string Client::extractNextCommand()
{
    if(!hasCompleteCommand())
        return "";
    //remove any empty line beginging of the buffer
    while(_buffer.find("\r\n") == 0)
        _buffer.erase(0, 2);
    //find teh first occurrence of \r\n to extract first command not last
    size_t found = _buffer.find("\r\n");
    //if no \r\n found return an empty command
    if(found == std::string::npos)
        return "";
    //copy the command without \r\n
    std::string cmd = _buffer.substr(0, found);
    //remove the command and \r\n from buffer
    _buffer.erase(0, found + 2);
    return cmd;
}
/* main to test the client cpp */
/*
int main()
{
    Client client(5, "127.0.0.1");

    std::cout << "=== TEST 1: Empty buffer ===" << std::endl;
    std::cout << client.hasCompleteCommand() << std::endl;

    std::cout << "\n=== TEST 2: Incomplete command ===" << std::endl;
    client.appendToBuffer("NICK");
    std::cout << client.hasCompleteCommand() << std::endl;

    std::cout << "\n=== TEST 3: Complete command ===" << std::endl;
    client.appendToBuffer(" bob\r\n");
    std::cout << client.hasCompleteCommand() << std::endl;

    std::string command = client.extractNextCommand();
    std::cout << "Extracted: [" << command << "]" << std::endl;

    std::cout << "\n=== TEST 4: Multiple commands ===" << std::endl;
    client.appendToBuffer("USER john 0 * :John\r\n");
    client.appendToBuffer("JOIN #42\r\n");

    while (client.hasCompleteCommand())
    {
        command = client.extractNextCommand();
        std::cout << "Extracted: [" << command << "]" << std::endl;
    }

    return 0;
}*/
/*
void printTitle(std::string const& title)
{
    std::cout << "\n========================================\n";
    std::cout << "TEST: " << title << std::endl;
    std::cout << "========================================\n";
}

void printResult(bool result, std::string const& message)
{
    std::cout << "\nResult: ";
    if (result)
        std::cout << "PASS - " << message << std::endl;
    else
        std::cout << "FAIL - " << message << std::endl;
}

int main()
{
    Client client;

    // ========================================
    // TEST 1: DEFAULT CONSTRUCTOR
    // ========================================

    printTitle("DEFAULT CONSTRUCTOR");

    std::cout << "Expected initial values:\n";
    std::cout << "  FD: -1\n";
    std::cout << "  Nickname: empty\n";
    std::cout << "  Username: empty\n";
    std::cout << "  Has password: false\n";
    std::cout << "  Has nickname: false\n";
    std::cout << "  Has username: false\n";
    std::cout << "  Registered: false\n";

    std::cout << "\nActual values:\n";
    std::cout << "  FD: " << client.getFd() << std::endl;
    std::cout << "  Nickname: [" << client.getNickName() << "]" << std::endl;
    std::cout << "  Username: [" << client.getUserName() << "]" << std::endl;
    std::cout << "  Has password: "
              << (client.getHasPass() ? "true" : "false") << std::endl;
    std::cout << "  Has nickname: "
              << (client.getHasNick() ? "true" : "false") << std::endl;
    std::cout << "  Has username: "
              << (client.getHasUser() ? "true" : "false") << std::endl;
    std::cout << "  Registered: "
              << (client.getIsRegistered() ? "true" : "false") << std::endl;

    printResult(
        client.getFd() == -1 &&
        client.getNickName().empty() &&
        client.getUserName().empty() &&
        !client.getHasPass() &&
        !client.getHasNick() &&
        !client.getHasUser() &&
        !client.getIsRegistered(),
        "Default values are correct"
    );

    // ========================================
    // TEST 2: SET NICKNAME
    // ========================================

    printTitle("SET NICKNAME");

    std::cout << "Before setting nickname:\n";
    std::cout << "  Nickname: [" << client.getNickName() << "]" << std::endl;
    std::cout << "  Has nickname: "
              << (client.getHasNick() ? "true" : "false") << std::endl;

    std::cout << "\nAction:\n";
    std::cout << "  setNickName(\"bob\")\n";
    client.setNickName("bob");
    client.setHasNick(true);
    std::cout << "\nAfter setting nickname:\n";
    std::cout << "  Nickname: [" << client.getNickName() << "]" << std::endl;
    
    std::cout << "  Has nickname: "
              << (client.getHasNick() ? "true" : "false") << std::endl;

    printResult(
        client.getNickName() == "bob",
        "Nickname changed from empty to bob"
    );

    // ========================================
    // TEST 3: SET USERNAME
    // ========================================

    printTitle("SET USERNAME");

    std::cout << "Before setting username:\n";
    std::cout << "  Username: [" << client.getUserName() << "]" << std::endl;
    std::cout << "  Has username: "
              << (client.getHasUser() ? "true" : "false") << std::endl;

    std::cout << "\nAction:\n";
    std::cout << "  setUserName(\"john\")\n";
    client.setUserName("john");
    client.setHasUser(true);

    std::cout << "\nAfter setting username:\n";
    std::cout << "  Username: [" << client.getUserName() << "]" << std::endl;
    std::cout << "  Has username: "
              << (client.getHasUser() ? "true" : "false") << std::endl;

    printResult(
        client.getUserName() == "john",
        "Username changed from empty to john"
    );

    // ========================================
    // TEST 4: SET PASSWORD FLAG
    // ========================================

    printTitle("SET PASSWORD FLAG");

    std::cout << "Before:\n";
    std::cout << "  Has password: "
              << (client.getHasPass() ? "true" : "false") << std::endl;

    std::cout << "\nAction:\n";
    std::cout << "  setHasPass(true)\n";
    client.setHasPass(true);

    std::cout << "\nAfter:\n";
    std::cout << "  Has password: "
              << (client.getHasPass() ? "true" : "false") << std::endl;

    printResult(client.getHasPass(), "Password flag changed to true");

    // ========================================
    // TEST 5: SET REGISTERED FLAG
    // ========================================

    printTitle("SET REGISTERED FLAG");

    std::cout << "Before:\n";
    std::cout << "  Registered: "
              << (client.getIsRegistered() ? "true" : "false") << std::endl;

    std::cout << "\nAction:\n";
    std::cout << "  setIsRegistered(true)\n";
    client.setIsRegistered(true);

    std::cout << "\nAfter:\n";
    std::cout << "  Registered: "
              << (client.getIsRegistered() ? "true" : "false") << std::endl;

    printResult(
        client.getIsRegistered(),
        "Registration flag changed to true"
    );

    // ========================================
    // TEST 6: BUFFER - INCOMPLETE COMMAND
    // ========================================

    printTitle("BUFFER - INCOMPLETE COMMAND");

    std::cout << "Action:\n";
    std::cout << "  appendToBuffer(\"NICK bob\")\n";
    client.appendToBuffer("NICK bob");

    std::cout << "\nCurrent buffer:\n";
    std::cout << "  [" << client.getBuffer() << "]" << std::endl;

    std::cout << "\nIs there a complete command? "
              << (client.hasCompleteCommand() ? "yes" : "no") << std::endl;

    printResult(
        !client.hasCompleteCommand(),
        "Incomplete command is correctly waiting for CRLF"
    );

    // ========================================
    // TEST 7: BUFFER - COMPLETE COMMAND
    // ========================================

    printTitle("BUFFER - COMPLETE COMMAND");

    std::cout << "Action:\n";
    std::cout << "  appendToBuffer(\"\\r\\n\")\n";
    client.appendToBuffer("\r\n");

    std::cout << "\nCurrent buffer:\n";
    std::cout << "  [" << client.getBuffer() << "]" << std::endl;

    std::cout << "\nIs there a complete command? "
              << (client.hasCompleteCommand() ? "yes" : "no") << std::endl;

    std::string command = client.extractNextCommand();

    std::cout << "\nExtracted command:\n";
    std::cout << "  [" << command << "]" << std::endl;

    std::cout << "Buffer after extraction:\n";
    std::cout << "  [" << client.getBuffer() << "]" << std::endl;

    printResult(
        command == "NICK bob",
        "Complete command was extracted correctly"
    );

    // ========================================
    // TEST 8: MULTIPLE COMMANDS
    // ========================================

    printTitle("MULTIPLE COMMANDS IN ONE BUFFER");

    std::cout << "Action:\n";
    std::cout << "  appendToBuffer(\"NICK alice\\r\\nUSER john 0 * :John\\r\\n\")\n";

    client.appendToBuffer("NICK alice\r\nUSER john 0 * :John\r\n");

    std::cout << "\nCurrent buffer:\n";
    std::cout << "  [" << client.getBuffer() << "]" << std::endl;

    std::cout << "\nExtracting commands:\n";

    while (client.hasCompleteCommand())
    {
        std::string cmd = client.extractNextCommand();
        std::cout << "  Extracted: [" << cmd << "]" << std::endl;
    }

    std::cout << "\nBuffer after extraction:\n";
    std::cout << "  [" << client.getBuffer() << "]" << std::endl;

    printResult(
        client.getBuffer().empty(),
        "All complete commands were extracted"
    );

    // ========================================
    // TEST 9: FRAGMENTED COMMAND
    // ========================================

    printTitle("FRAGMENTED COMMAND");

    std::cout << "First received data: \"NI\"\n";
    client.appendToBuffer("NI");

    std::cout << "Buffer: [" << client.getBuffer() << "]" << std::endl;
    std::cout << "Complete command: "
              << (client.hasCompleteCommand() ? "yes" : "no") << std::endl;

    std::cout << "\nSecond received data: \"CK alice\\r\\n\"\n";
    client.appendToBuffer("CK alice\r\n");

    std::cout << "Buffer: [" << client.getBuffer() << "]" << std::endl;
    std::cout << "Complete command: "
              << (client.hasCompleteCommand() ? "yes" : "no") << std::endl;

    command = client.extractNextCommand();

    std::cout << "\nExtracted command:\n";
    std::cout << "  [" << command << "]" << std::endl;

    printResult(
        command == "NICK alice",
        "Fragments were combined into one complete command"
    );

    std::cout << "\n========================================\n";
    std::cout << "ALL TESTS FINISHED\n";
    std::cout << "========================================\n";

    return 0;
}*/