
#ifndef __PROXY_H_
#define __PROXY_H_
#include <cstdlib>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>
using namespace std;
class Proxy{
private:
	int webserver_fd;//fd for website
	int client_fd;// fd for user
	int listen_fd;//dummy fd

	struct addrinfo host_info;
	struct addrinfo *host_info_list;

	string hostname;
	string proxy_port;//for user to send request
	string webserver_port;//for proxy to send request to web
public:
	void getAddressInfo();
	void createSocketFd();
	void startListening();//set, bind, listen
	void acceptConnection();//get the socket fd of accept

	void connectWebServer();//connect to webserver

	int getClientFd(){return client_fd};
	int getListenFd(){return listen_fd};
	int getWebServerFd(){return webserver_fd};

	Proxy():webserver_fd(-1),client_fd(-1),listen_fd(-1),hostname(""),proxy_port("4444"),webserver_port("80"){};


};

#endif

