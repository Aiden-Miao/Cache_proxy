#ifndef __LOG__
#define __LOG__

//#include"proxy.hpp"
//#include"handler.hpp"
//#include"request_parser.hpp"
//#include"response_parser.hpp"
#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <ostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "request_parser.hpp"
#include "response_parser.hpp"
using namespace std;
class log{
public:
	void writeNewRequest(size_t thread_id, string request, char* ip){
		time_t requesttime;
		struct tm * realtime;
		time(&requesttime);
		realtime = localtime(&requesttime);
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< request << '"' << " from " << ip << " @ " << asctime(realtime) << endl;
		file.close();
	}

	void writeRequestWebserver(size_t thread_id, RequestParser web_request, char * ip){
		string web_hostname = web_request.getWebHostname();
		string request_content = web_request.getfirstline();
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< " Requesting " << request_content << " from " << web_hostname<< endl;
		file.close();
	}

	void writeResponseWebserver(size_t thread_id, ResponseParser web_response, char * ip){
		string web_hostname = web_response.getWebHostname();
		string response_content = web_response.getfirstline();
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< " Received " << response_content << " from " << web_hostname<< endl;
		file.close();	
	}

	void writeResponseClient(size_t thread_id, ResponseParser client_response, char * ip){
		string web_hostname = client_response.getWebHostname();
		string response_content = client_response.getfirstline();
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< " Responding " << response_content << endl;
		file.close();	
	}


};

#endif
