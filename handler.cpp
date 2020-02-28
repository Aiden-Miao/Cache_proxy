#include "handler.hpp"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/time.h>
#include<time.h>
#include<sstream>
#include<exception>
#include<assert.h>
using namespace std;

void Handler::connectWebServer(const char *hostname, const char * port){
  int status;
  struct addrinfo remote_info;
  struct addrinfo *remote_info_list = NULL;
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

void Handler::sendToFd(int fd,string to_send){
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

string Handler::receiveHeader(int fd){
  vector<char> header(1, 0);
  int index = 0;
  int nbytes;
  while ((nbytes = recv(fd, &header.data()[index], 1 ,MSG_WAITALL)) > 0) {
    if (header.size() > 4) {
      if (header.back() == '\n' && header[header.size() - 2] == '\r' &&
          header[header.size() - 3] == '\n' &&
          header[header.size() - 4] == '\r') {
          break;
      }
    }
    header.resize(header.size() + 1);
    index += nbytes;
  }
  string ans(header.begin(),header.end());
  return ans;
}

string Handler::receiveContent(int fd,int content_length){
  //cout<<"Initial Content-length = "<<content_length<<endl;
  int original_length = content_length;
  vector<char> content(1,0);
  int index = 0;
  int nbytes = 0;
  //while ((nbytes = recv(fd, &content.data()[index], 1 ,MSG_WAITALL)) > 0){
  while (true){
    nbytes = recv(fd, &content.data()[index], 1 ,MSG_WAITALL);
    if(nbytes==0){
      break;
    }
    if(nbytes==-1){
      continue;
    }
    index += nbytes;
    content_length -= nbytes;
    if(content_length==0){
      break;
    }
    content.resize(content.size()+1);
  }
  //cout<<"After recv, content_length = "<<content_length<<", nbytes = "<<nbytes<<endl;
  //assert(content_length==0);// content_length !=0
  string ans(content.begin(),content.end());
  if((int)ans.size()!=original_length){
    logic_error e("Dirty content!");
    throw exception(e);
  }
  return ans;
}

void Handler::handleGET(int client_fd, RequestParser req_parser, size_t id){//should this be refrence?
  if(req_parser.getContentLength()!="0"){
    //cout<<"*****Enter recv normal content*****"<<endl;
    stringstream ss;
    ss<<req_parser.getContentLength();
    int content_length;
    ss>>content_length;
    string content = receiveContent(client_fd,content_length);
    req_parser.addContent(content);
    //cout<<"After addContent, content is:\n"<<req_parser.getContent()<<endl;
  }

  connectWebServer(req_parser.getWebHostname().c_str(),req_parser.getWebPort().c_str());
  sendToFd(getWebServerFd(),req_parser.getRequest());
  // sendToFd(getWebServerFd(),req_parser.getHeader());
  // sendToFd(getWebServerFd(),req_parser.getContent());
  
  string response_header = receiveHeader(getWebServerFd());
  cout<<"This is response_header\n"<<response_header;
  ResponseParser resp_parser(response_header);
  resp_parser.parseHeader();

  if(resp_parser.getContentLength()!="0"){ //content length
    stringstream ss;
    ss<<resp_parser.getContentLength();
    int response_content_len;
    ss>>response_content_len;
    string content = receiveContent(getWebServerFd(),response_content_len);
    resp_parser.addContent(content);
    // sendToFd(client_fd,resp_parser.getHeader());
    // sendToFd(client_fd,resp_parser.getContent());
    sendToFd(client_fd,resp_parser.getResponse());
  }
  else if(resp_parser.getIsChunked()){  //chunked
    string content = recvChunkedContent(webserver_fd);
    //cout<<"****nmsl****"<<endl;
    //cout<<content<<endl;
    resp_parser.addContent(content);
    sendToFd(client_fd,resp_parser.getResponse());
    // sendToFd(client_fd,resp_parser.getHeader());
    // sendToFd(client_fd,resp_parser.getContent());
  }
  else{
    //sendToFd(client_fd,resp_parser.getResponse());
    vector<char> recv_buf(1024,0);
    int nbytes = loopRecv(recv_buf,getWebServerFd());
    if(nbytes==0){
      sendToFd(client_fd,resp_parser.getResponse());
    }
    else{
      sendToFd(client_fd,resp_parser.getResponse());
      loopSend(recv_buf,client_fd,nbytes);
    }
  }

}

void Handler::handlePOST(int client_fd,RequestParser req_parser, size_t id){ //should this be refrence?
  if(req_parser.getContentLength()!="0"){ //normal content
    stringstream ss;
    ss<<req_parser.getContentLength();
    int content_length;
    ss>>content_length;
    string content = receiveContent(client_fd,content_length);
    req_parser.addContent(content);
    //cout<<"After addContent, content is:\n"<<req_parser.getContent()<<endl;
  }
  else if(req_parser.getIsChunked()){ //chunk content
    string content = recvChunkedContent(client_fd);
    req_parser.addContent(content);
  }
  else{
    vector<char> recv_buf(1024,0);
    int nbytes = loopRecv(recv_buf,client_fd);
    if(nbytes > 0){
      string content(recv_buf.begin(),recv_buf.end());
      req_parser.addContent(content);
    }
  }

  cout<<"*******the post request is "<<req_parser.getRequest()<<"***********"<<endl;
  connectWebServer(req_parser.getWebHostname().c_str(),req_parser.getWebPort().c_str());
  
  //sendToFd(getWebServerFd(),req_parser.getHeader());
  //sendToFd(getWebServerFd(),req_parser.getContent());
  sendToFd(getWebServerFd(),req_parser.getRequest());
  
  //below is response 
  string response_header = receiveHeader(getWebServerFd());
  
  ResponseParser resp_parser(response_header);
  
  resp_parser.parseHeader();
  
  if(resp_parser.getContentLength()!="0"){
    stringstream ss;
    ss<<resp_parser.getContentLength();
    int response_content_len;
    ss>>response_content_len;
    string content = receiveContent(getWebServerFd(),response_content_len);
    //cout<<"*******3"<<endl;
    resp_parser.addContent(content);
    // sendToFd(client_fd,resp_parser.getHeader());
    // sendToFd(client_fd,resp_parser.getContent());
    //cout<<"*******4"<<endl;
  }
  else if(resp_parser.getIsChunked()){
    string content = recvChunkedContent(getWebServerFd());
    resp_parser.addContent(content);
    // sendToFd(client_fd,resp_parser.getHeader());
    // sendToFd(client_fd,resp_parser.getContent());
  }
  else{
    vector<char> recv_buf(1024,0);
    int nbytes = loopRecv(recv_buf,getWebServerFd());
    if(nbytes > 0){
      string content(recv_buf.begin(),recv_buf.end());
      resp_parser.addContent(content);
    }
  }
  sendToFd(client_fd,resp_parser.getResponse());
  // sendToFd(client_fd,resp_parser.getHeader());
  // sendToFd(client_fd,resp_parser.getContent());
  
}

void Handler::handleCONNECT(int client_fd,RequestParser req_parser, size_t id){//should this be refrence?
  //cout<<"In CONNECT, web hostname = "<<req_parser.getWebHostname()<<" ,web port = "<<req_parser.getWebPort();
  if(req_parser.getWebHostname()==""){
    return;
  }
  cout<<"=======1"<<endl;
  connectWebServer(req_parser.getWebHostname().c_str(),req_parser.getWebPort().c_str());
  cout<<"=======2"<<endl;
  //cout<<"Connect web server success! id = "<<id<<endl;
  string success_msg = "HTTP/1.1 200 OK\r\n\r\n";
  sendToFd(client_fd,success_msg);
  cout<<"=======3"<<endl;
  int status = 0;
  fd_set readfds;
  int fdmax = max(client_fd,getWebServerFd());
  int fds[2] = {client_fd,getWebServerFd()};
  while(true){
    FD_ZERO(&readfds);
    FD_SET(fds[0],&readfds);
    FD_SET(fds[1],&readfds);
    status = select(fdmax+1,&readfds,NULL,NULL,NULL);
    if(status == -1){
      cerr<<"Select failed!"<<endl;
      logic_error e("select failed!");
      throw exception(e);
    }
    // if(status == 0){ //2.27 4:40 pm
    //   return;
    //   //continue;
    // }

    //bool destroy_tunnel = false // if true, destroy the CONNECT tunnel
    for(size_t i = 0; i < 2; i++){
      if(FD_ISSET(fds[i],&readfds)){
        vector<char> recv_buf(1024,0);
        int nbytes = loopRecv(recv_buf, fds[i]);//get the data recieved
        //cout<<"receive success!"<<endl;
        if(nbytes == 0){ // destroy tunnel
          return;
        }
        loopSend(recv_buf, fds[1-i], nbytes);//send all data out
        //cout<<"send success!"<<endl;
        break;
      }
    }
  }
}

string Handler::recvChunkedContent(int fd){
  vector<char> buf(1, 0);
  int index = 0;
  int nbytes;
  //while ((nbytes = recv(fd, &buf.data()[index], 1 ,MSG_WAITALL)) >= 0) {
  while (true) {
    nbytes = recv(fd, &buf.data()[index], 1 ,MSG_WAITALL);
    if(nbytes==0){
      break;
    }
    if(nbytes==-1){
      continue;
    }
    if (buf.size() > 5) {
      if (buf.back() == '\n' && buf[buf.size() - 2] == '\r' &&
          buf[buf.size() - 3] == '\n' &&
          buf[buf.size() - 4] == '\r' && buf[buf.size() - 5] == '0') {
          cout<<"****Encounter 0,r,n******"<<endl;
          // std::cout << "GOT HEADER!" << std::endl;
          //find = 1;
          break;
      }
    }
    buf.resize(buf.size() + 1);
    index += nbytes;
  }
  string ans(buf.begin(),buf.end());
  return ans;
}


int Handler::loopRecv(vector<char> & recv_buf,int fd){
  //cout<<"+++++Enter loopRecv"<<endl;
  int nbytes = 0; //total bytes received
  int byte_recv = 0; // bytes received in each iteration
  int prev_recv_size = 0;
  while((byte_recv = recv(fd, &recv_buf.data()[nbytes],(int)recv_buf.size()-prev_recv_size,0))==(int)recv_buf.size()){
    prev_recv_size = (int)recv_buf.size();
    recv_buf.resize(2*recv_buf.size());
    nbytes += byte_recv;
    continue;
  }
  nbytes+=byte_recv;
  recv_buf.resize(nbytes);
  return nbytes;
}

//continue send date until all is sent
void Handler::loopSend(vector<char> & recv_buf, int fd, int byte_size){
  //cout<<"+++++Enter loopSend"<<endl;
  int byte_sent = 0;
  while(byte_sent < byte_size){
    byte_sent += send(fd, &recv_buf.data()[byte_sent], byte_size - byte_sent, 0);
  }
  //cout<<"In loop send, byte_sent = "<<byte_sent<<", total_byte = "<<byte_size<<endl;
  //assert(byte_sent == byte_size);
} 