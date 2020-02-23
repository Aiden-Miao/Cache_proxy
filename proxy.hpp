#ifndef __PROXY__
#define __PROXY__
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
class Proxy{
private:
	int webserver_fd;//fd for website
	int client_fd;// fd for user
	int listen_fd;//dummy fd

	struct addrinfo host_info;
	struct addrinfo *host_info_list;
	struct addrinfo remote_info;
	struct addrinfo *remote_info_list;

	const char * hostname;
	const char * proxy_port;//for user to send request
	const char * webserver_port;//for proxy to send request to web
public:
	void getAddressInfo();
	void createSocketFd();
	void startListening();//set, bind, listen
	void acceptConnection();//get the socket fd of accept

	void connectWebServer(const char *hostname, const char * port);//connect to webserver

	int getClientFd(){return client_fd;}
	int getListenFd(){return listen_fd;}
	int getWebServerFd(){return webserver_fd;}

	void testProxy();

	string receive_request();

	Proxy():webserver_fd(-1),client_fd(-1),listen_fd(-1),hostname(NULL),proxy_port("4444"),webserver_port("80"){};
	~Proxy(){
		close(webserver_fd);
		close(client_fd);
		close(listen_fd);
		freeaddrinfo(host_info_list);
		//freeaddrinfo(remote_info_list);
	}

};

#endif

