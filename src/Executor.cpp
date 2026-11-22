
#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Commands.hpp"

Executor::Executor()
{
}

void Executor::execute(Server& server, Client& client,
                              const std::string& message)
{
    std::istringstream iss(message);
    std::string command;
    iss >> command;

    if(command.empty())
        return;
    for (size_t i = 0; i < command.size(); ++i)
        command[i] = std::toupper(command[i]);
    if (command == "PASS")
        executePass(server, client, iss);
    else if (command == "NICK")
        executeNick(server, client, iss);
    else if (command == "USER")
        executeUser(server, client, iss);
    else if(command == "JOIN")
        executeJoin(server, client, iss);
    else if (command == "NAMES")
        executeNames(server, client, iss);
    else if (command == "PRIVMSG")
        executePrivmsg(server, client, iss);
    else if(command == "PART")
       executePart(server, client, iss);
    else if (command == "KICK")
        executeKick(server, client, iss);
    else if(command == "TOPIC")
        executeTopic(server, client, iss);
    else if(command == "INVITE")
        executeInvite(server, client, iss);
    else if(command == "MODE")
        executeMode(server, client, iss);
    else if (command == "PING")
        executePing(server, client, iss);
    else if (command == "QUIT")
        executeQuit(server, client, iss);
    else if(command == "CAP")
        executeCap(server, client, iss);
    else if(command == "PONG")
        ;
    else
        sendMessage(server, client, 421, "unknown command");
}
