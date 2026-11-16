#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "Client.hpp"
#include "Executor.hpp"
#include <sstream>
#include <string>
class Server;
class Client;
class Executor;

void executePass(Server& server, Client& client, std::istringstream& iss);
void executeNick(Server& server, Client& client, std::istringstream& iss);
void executeUser(Server& server, Client& client, std::istringstream& iss);
void executeJoin(Server& server, Client& client, std::istringstream& iss);
void executePrivmsg(Server& server, Client& client, std::istringstream& iss);
void executePart(Server& server, Client& client, std::istringstream& iss);
void executeTopic(Server& server, Client& client, std::istringstream& iss);
void executeKick(Server& server, Client& client, std::istringstream& iss);

#endif