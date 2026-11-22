
#include "Server.hpp"
#include "Client.hpp"
#include "Executor.hpp"

void executePing(Server& server, Client& client, std::istringstream& iss)
{
    std::string tokenPing;
    iss >> tokenPing;
    if (tokenPing.empty())
    {
        sendMessage(server, client, 409, "No origin specified");
        return;
    }
    if (tokenPing[0] == ':')
        tokenPing.erase(0, 1);
    if (tokenPing.empty())
    {
        sendMessage(server, client, 409, "No origin specified");
        return;
    }
    std::ostringstream oss;
    oss << ":" << "ircserv" << " PONG "
        << "ircserv" << " :" << tokenPing << "\r\n";
    server.sendMessageToClient(client.getFd(), oss.str());
}

void	executeQuit(Server& server, Client& client, std::istringstream& iss)
{
	std::string quitMessage;
	std::getline(iss, quitMessage);
	while (!quitMessage.empty() && quitMessage[0] == ' ')
        quitMessage.erase(0, 1);
    if(!quitMessage.empty() && quitMessage[0] == ':')
        quitMessage.erase(0, 1);
    if(quitMessage.empty())
		quitMessage = "Client Quit";
	server.notifySharedChannelsQuit(client, quitMessage);
	server.handleClientDisconnection(client.getFd());
}

void executeCap(Server& server, Client& client, std::istringstream& iss)
{
    std::string subCommand;
    iss >> subCommand;
    std::string target = client.getNickName();
    if (target.empty())
        target = "*";
    if (subCommand == "LS")
    {
        server.sendMessageToClient(client.getFd(),
            ":localhost CAP " + target + " LS :multi-prefix\r\n");
    }
    else if (subCommand == "LIST")
    {
        std::string capabilities;
        const std::vector<std::string>& caps = client.getCapabilities();
        for (std::vector<std::string>::const_iterator it = caps.begin();
             it != caps.end(); ++it)
        {
            if (!capabilities.empty())
                capabilities += " ";
            capabilities += *it;
        }
        server.sendMessageToClient(client.getFd(),
            ":localhost CAP " + target + " LIST :" + capabilities + "\r\n");
    }
    else if (subCommand == "REQ")
    {
        std::string requested;
        std::getline(iss, requested);
        size_t pos = requested.find_first_not_of(" \t");
        if (pos != std::string::npos)
            requested = requested.substr(pos);
        else
            requested.clear();
        if (!requested.empty() && requested[0] == ':')
            requested.erase(0, 1);
        pos = requested.find_first_not_of(" \t");
        if (pos != std::string::npos)
            requested = requested.substr(pos);
        else
            requested.clear();
        pos = requested.find_last_not_of(" \t");
        if (pos != std::string::npos)
            requested.erase(pos + 1);
        std::istringstream reqStream(requested);
        std::vector<std::string> tokens;
        std::string token;
        bool valid = true;
        while (reqStream >> token)
        {
            std::string capability = token;

            if (capability[0] == '+' || capability[0] == '-')
                capability.erase(0, 1);

            if (capability != "multi-prefix")
                valid = false;
            tokens.push_back(token);
        }
        if (tokens.empty())
            valid = false;
        if (!valid)
        {
            server.sendMessageToClient(client.getFd(),
                ":localhost CAP " + target + " NAK :" + requested + "\r\n");
            return;
        }
        for (std::vector<std::string>::iterator it = tokens.begin();
             it != tokens.end(); ++it)
        {
            if ((*it)[0] == '-')
            {
                std::string capability = it->substr(1);
                client.removeCapability(capability);
            }
            else
            {
                std::string capability = *it;

                if (capability[0] == '+')
                    capability.erase(0, 1);

                client.addCapability(capability);
            }
        }
        server.sendMessageToClient(client.getFd(),
            ":localhost CAP " + target + " ACK :" + requested + "\r\n");
    }
    else if (subCommand == "END")
    {
        return;
    }
}

