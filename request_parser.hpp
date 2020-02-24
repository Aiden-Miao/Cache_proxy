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
class Request_Parser{
private:
	string header;
	string method;//(0,first blank space)
	string url;//url of the website
	string web_hostname;//the hostname of the website:www.example.com
	string web_port;//default 80

public:
	Request_Parser(string head):header(head),web_hostname(""),web_port(""){}
	string getHeader(){return header;}
	string getMethod(){return method;}
	string getUrl(){return url;}
	string getWebHostname(){return web_hostname;}
	string getWebPort(){return web_port;}
	void parse_Header();
};

// class RequestParser:public Parser{
// 	int client_fd;
// };

// class ResponseParser:public Parser{

// };