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
using namespace std;
class log{
public:
	void writeRequest(size_t thread_id, string request, char* ip){
		time_t requesttime;
		struct tm * realtime;
		time(&requesttime);
		realtime = localtime(&requesttime);
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< request << '"' << " from " << ip << " @ " << asctime(realtime) << endl;
		file.close();
	}


};

#endif
