#include"proxy.hpp"
#include"handler.hpp"
#include"request_parser.hpp"
#include"response_parser.hpp"
#include<stdlib.h>
#include<cstdlib>
#include<sstream>
#include<exception>
#include<thread>
#include<csignal>
using namespace std;
class log{
	void writeRequest(size_t thread_id, string request, string ip){
		time_t requesttime;
		struct tm * time;
		time(&requesttime);
		time = localtime(&requesttime);
		ofstream file;
		file.open("proxy.log", ios::app|ios::out);
		file << thread_id << " : " << '"'<< request << '"' << "from" << ip << " @ " << asctime(time) << endl;
		file.close();
	}


};