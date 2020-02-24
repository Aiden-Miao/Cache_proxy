#include "proxy.hpp"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<assert.h>
using namespace std;
void Proxy::getAddressInfo(){
	int status;
 	//const char *hostname = NULL;
  	//const char *port     = "4444";
 	memset(&host_info, 0, sizeof(host_info));
	host_info.ai_family   = AF_UNSPEC;
  	host_info.ai_socktype = SOCK_STREAM;
  	host_info.ai_flags    = AI_PASSIVE;
	status = getaddrinfo(hostname, proxy_port, &host_info, &host_info_list);
	if (status != 0) {
    	cerr << "Error: cannot get address info for host" << endl;
    //	cerr << "  (" << hostname << "," << port << ")" << endl;
    	exit(EXIT_FAILURE);
  	}
}

void Proxy::createSocketFd(){
	listen_fd = socket(host_info_list->ai_family, 
					host_info_list->ai_socktype, 
					host_info_list->ai_protocol);
	if (listen_fd == -1) {
    	cerr << "Error: cannot create socket" << endl;
    //	cerr << "  (" << hostname << "," << port << ")" << endl;
    	exit(EXIT_FAILURE);
  }
}

void Proxy::startListening(){
	int status;
	int yes = 1;
	status = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	status = bind(listen_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1) {
    	cerr << "Error: cannot bind socket" << endl;
    //	cerr << "  (" << hostname << "," << port << ")" << endl;
    	exit(EXIT_FAILURE);
	} //if
	status = listen(listen_fd, 100);
	if (status == -1) {
    	cerr << "Error: cannot listen on socket" << endl; 
    //	cerr << "  (" << hostname << "," << port << ")" << endl;
    	exit(EXIT_FAILURE);
	} //if
}//set, bind, listen

void Proxy::acceptConnection(){
	struct sockaddr_storage socket_addr;
	socklen_t socket_addr_len = sizeof(socket_addr);
	client_fd = accept(listen_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
	cout<<"Shit! Accept success!"<<endl;
	if (client_fd == -1) {
    	cerr << "Error: cannot accept connection on socket" << endl;
    	exit(EXIT_FAILURE);
	} //if
}//get the socket fd of accept

void Proxy::connectWebServer(const char *hostname, const char * port){
  int status;
  memset(&remote_info, 0, sizeof(remote_info));
  remote_info.ai_family   = AF_UNSPEC;
  remote_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &remote_info, &remote_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for webserver" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if

  webserver_fd = socket(remote_info_list->ai_family, 
		     remote_info_list->ai_socktype, 
		     remote_info_list->ai_protocol);
  if (webserver_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(webserver_fd, remote_info_list->ai_addr, remote_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    //cerr << "  (" << hostname << "," << port << ")" << endl;
    exit(EXIT_FAILURE);
  } //if
}//connect to webserver

void Proxy::sendToFd(int fd,string to_send){
  int total_size = to_send.size();
  if(total_size==0){
    return;
  }
  int nbytes = 0;
  while(true){
    nbytes += send(fd,to_send.substr(nbytes).c_str(),total_size-nbytes,0);
    cout<<"Send "<<nbytes<<" Bytes"<<endl;
    if(nbytes==total_size){
      break;
    }
  }
  assert(nbytes==total_size);
}

string Proxy::receiveHeader(){
  vector<char> header(1, 0);
  int index = 0;
  int nbytes;
  while ((nbytes = recv(client_fd, &header.data()[index], 1 ,MSG_WAITALL)) > 0) {
    if (header.size() > 4) {
      if (header.back() == '\n' && header[header.size() - 2] == '\r' &&
          header[header.size() - 3] == '\n' &&
          header[header.size() - 4] == '\r') {
          // std::cout << "GOT HEADER!" << std::endl;
          //find = 1;
          break;
      }
    }
    header.resize(header.size() + 1);
    index += nbytes;
  }
  string ans(header.begin(),header.end());
  return ans;
}

string Proxy::receiveContent(int content_length){
  vector<char> content(1,0);
  int index = 0;
  int nbytes;
  while ((nbytes = recv(client_fd, &content.data()[index], 1 ,MSG_WAITALL)) > 0){
    content.resize(content.size()+1);
    index += nbytes;
    content_length -= nbytes;
    if(content_length==0){
      break;
    }
  }
  assert(content_length==0);
  string ans(content.begin(),content.end());
  return ans;
}





























