
#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <exception>
#include <string>
#include <unistd.h>
#include <cerrno>
#include <sstream>
#include <cstdlib>

class Server
{
	private:
		int _port;
		std::string _password;
		int _fd;
	public:
		Server(int port, const std::string& pass);
		~Server();
		
		void run();
};

#endif
