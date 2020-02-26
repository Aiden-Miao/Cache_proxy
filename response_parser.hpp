#ifndef __RESPONSE_PARSER__
#define __RESPONSE_PARSER__
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
 
using namespace std;
class ResponseParser
{
public:
	string header;
	string content_length;
	string content;
	string response;//the whole response
	string status; //200 OK
	string status_num; //200
	string cache_control; //if cacheable and the existing time
	string expire; //when it will expire
	string date; //when the response is sent
	string last_modified; //last modified time
	string E_tag;
	string age;
	int chunk;///???????????
	int status_valid; //use to determine if failed
	//int age; //time existed in cache
	//int content_length; //length of content

	//constructor
	ResponseParser(string recv_header):	header(recv_header),
										content_length(""),
										content(""),
										response(recv_header),
										status(""),
										status_num(""),
										cache_control(""),
										expire(""),
										date(""),
										last_modified(""),
										E_tag(""),
										age(""),
										chunk(0),
										status_valid(0)
										//content_length(0),
										{}
	string getHeader(){return header;}
	string getContentLength(){return content_length;}
	string getContent(){return content;}
	string getStatus(){return status;}
	void parseHeader();
	void addContent(string content);

	~ResponseParser(){};
	friend class Proxy;
};

#endif