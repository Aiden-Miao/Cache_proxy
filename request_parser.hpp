#ifndef __REQUEST_PARSER__
#define __REQUEST_PARSER__
#include "cache.hpp"
//#include "proxy.hpp"
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
using namespace std;
class RequestParser{
private:
	string header; // the whole request header
	string method;// (0,first blank space)
	string url;//url of the website
	string web_hostname;//the hostname of the website: www.example.com
	string web_port;// default 80
	string content_length; // length of the content
	string content;
	string request;//the whole request
	bool is_chunked;// indicates the request is chunked or not

public:
	RequestParser(string recv_header):header(recv_header),web_hostname(""),web_port("80"),
				 content_length(""),content(""),request(recv_header),is_chunked(false){}
	string getHeader(){return header;}
	string getMethod(){return method;}
	string getUrl(){return url;}
	string getWebHostname(){return web_hostname;}
	string getWebPort(){return web_port;}
	string getContentLength(){return content_length;}
	string getContent(){return content;}
	bool getIsChunked(){return is_chunked;}
	void addContent(string content);
	void parseHeader();

friend class Proxy;
};
#endif