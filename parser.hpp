#include "cache.hpp"
#include "proxy.hpp"
#include <cstdlib>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
class Parser{
	virtual void parse() = 0;
};

class RequestParser:public Parser{
	int client_fd;
};

class ResponseParser:public Parser{

};