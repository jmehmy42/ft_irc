
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <exception>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <sstream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <cctype>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <map>
#include <csignal>
#include <sstream>
#include "Client.hpp"
#include "Executor.hpp"
#include "Channel.hpp"
#include "Commands.hpp"

class Channel;

class Server
{
    private:
        int _port;
        std::string _password;
        int _fd;
        int _running;
        Executor _executor;

        std::map<int, Client>           _clients; // easy to find client using fd
	    std::vector<struct pollfd>      _pollfds;
        std::map<std::string, Channel*> _channels;
        // Server(const Server& other);
        // Server& operator=(const Server& other);
        void closeSocketFd();

    public:
        Server(int port, const std::string& pass);
        ~Server();
        void run();
        void handleNewConnection();
        void handleClientMessage(int clientfd);
        void handleClientDisconnection(int clientfd);
        void signalHandler(int signum);
        void sendMessageToClient(int clientfd, const std::string& message);
        const std::string& getPassword()const;
        bool    isNickNameTaken(const std::string& nickname, int clientfd) const;
        std::map<std::string, Channel*>&    getChannels();
        std::map<std::string, Channel*>::iterator getChannel(const std::string& name);
        void    createChannel(const std::string& name, Client& newClient);
        void    removeChannel(const std::string& name);
        std::map<int, Client>&  getClients();
        const std::map<int, Client>& getClients()const;
        Client* getClientByNickname(const std::string& nickname);
        void    broadcastMessage(const std::string& message, int excludeFd);
		void	notifySharedChannelsQuit(const Client& client, const std::string& quitMessage);
};
    void sendMessage(Server& server, Client& client, int code, const std::string& text);

#endif
