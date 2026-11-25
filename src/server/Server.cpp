
#include "Server.hpp"
#include "Executor.hpp"
#include "Commands.hpp"

Server* g_server = NULL;

static void handleSignal(int signum);

Server::Server(int port, const std::string& pass)
    : _port(port), _password(pass), _fd(-1), _running(true)
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(_fd == -1)
		throw std::runtime_error("socket creation failed!");
    if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to set non-blocking mode");
    }
    int opt = 1;
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to set socket options");
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);
    if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to bind socket");
    }
    
    if (listen(_fd, SOMAXCONN) == -1)
    {
        closeSocketFd();
        throw std::runtime_error("Failed to listen");
    }
    std::cout << "Server listening on port "
              << _port << std::endl;
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handleSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
	g_server = this;
}

Server::~Server()
{
    g_server = NULL;
	for(size_t i = 0; i < _pollfds.size(); ++i)
    {
        if(_pollfds[i].fd != _fd)
            close(_pollfds[i].fd);
    }
    _clients.clear();
    _pollfds.clear();
    for(std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
        delete it->second;
    _channels.clear();
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

void Server::closeSocketFd()
{
    if(_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

void Server::run()
{
    struct pollfd pfd;
    pfd.fd = _fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollfds.push_back(pfd);
    while (_running)
    {
        updatePollEvents();
        int activity = poll(_pollfds.data(), _pollfds.size(), -1);
        if( activity < 0)
        {
            if(errno == EINTR)
                break;
            throw std::runtime_error("Poll failed!");
        }
        if (_pollfds[0].revents & POLLIN)
            handleNewConnection();
        for (size_t i = 1; i < _pollfds.size();)
        {
            int clientfd = _pollfds[i].fd;
            short revents = _pollfds[i].revents;
            if(revents & (POLLERR | POLLHUP | POLLNVAL))
            {
                handleClientDisconnection(clientfd);
                continue;
            }
            if(revents & POLLIN)
                handleClientMessage(clientfd);
            if(_clients.find(clientfd) == _clients.end())
                continue;
            if(revents & POLLOUT)
                handleClientWrite(clientfd);
            if(_clients.find(clientfd) == _clients.end())
                continue;
            i++;
        }
    }
}

static void handleSignal(int signum)
{
    if(g_server)
        g_server->signalHandler(signum);
}

void Server::signalHandler(int signum)
{
    _running = false;
    std::cout << " Signal interrupt! "<< signum << " Server is shutting down." << std::endl;
}

const std::string& Server::getPassword()const
{
    return _password;
}

void Server::updatePollEvents()
{
    for(size_t i = 1; i < _pollfds.size(); ++i)
    {
        int clientfd = _pollfds[i].fd;
        std::map<int, Client>::iterator it ;
        it = _clients.find(clientfd);
        if(it == _clients.end())
            continue;
        _pollfds[i].events = POLLIN;
        if(!it->second.getOutputBuffer().empty())
            _pollfds[i].events |= POLLOUT;
    }
}