#ifndef __HANDLER__
#define __HANDLER__
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
#include "request_parser.hpp"
#include "response_parser.hpp"
using namespace std;

class Handler{
private:
	int webserver_fd;
	// struct addrinfo remote_info;
	// struct addrinfo *remote_info_list = NULL;
	const char * webserver_port;//for handler to send request to web
public:
	void connectWebServer(const char *hostname, const char * port);//connect to webserver
	void sendToFd(int fd,string to_send);
	//int getClientFd(){return client_fd;}
	//int getListenFd(){return listen_fd;}
	int getWebServerFd(){return webserver_fd;}
	string receiveHeader(int fd); //recv the header from certain fd
	string receiveContent(int fd,int content_length); //recv the content
	int loopRecv(vector<char> & recv_buf,int fd);
	void loopSend(vector<char> & recv_buf, int fd, int byte_size);

	void handleGET(int client_fd,RequestParser &req_parser, size_t id);
	void handlePOST(int client_fd,RequestParser &req_parser, size_t id);
	void handleCONNECT(int client_fd,RequestParser &req_parser, size_t id);
	Handler(){}
	~Handler(){
		if(webserver_fd!=-1){
			close(webserver_fd);
		}
	}

};

#endif