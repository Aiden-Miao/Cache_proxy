#include"proxy.hpp"
#include"request_parser.hpp"
#include<sstream>
using namespace std;

void workHorse(Proxy proxy, int client_fd,size_t id);
void test(Proxy &proxy){
	proxy.acceptConnection();
	string header = proxy.receiveHeader();
	cout<<"This is header:\n"<<header<<endl;
	RequestParser req_parser(header);
	req_parser.parseHeader();
	if(req_parser.getContentLength().size()!=0){
		stringstream ss;
		ss<<req_parser.getContentLength();
		int content_length;
		ss>>content_length;
		string content = proxy.receiveContent(content_length);
		req_parser.addContent(content);
		cout<<"After addContent, content is:\n"<<req_parser.getContent()<<endl;
	}
	proxy.connectWebServer(req_parser.getWebHostname().c_str(),req_parser.getWebPort().c_str());
	cout<<"Connect web server success!"<<endl;
	proxy.sendToFd(proxy.getWebServerFd(),req_parser.getHeader());
	proxy.sendToFd(proxy.getWebServerFd(),req_parser.getContent());
	cout<<"Send to web server success!"<<endl;
}
int main(){
	Proxy proxy;
	// create a cache
	proxy.getAddressInfo();
	proxy.createSocketFd();
	proxy.startListening();//set, bind, listen
	//****Test start
	test(proxy);
	//****Test end

	// int listen_fd = proxy.getListenFd();
	// cout<<"Start listening on port 4444..."<<endl;
	// size_t id = 0;
	// signal(SIGPIPE,SIG_IGN);
	// while(true){
	// 	proxy.acceptConnection();
	// 	if((int client_fd = proxy.getClientFd())==-1){
	// 		continue;
	// 	}
	// 	try{
	// 		thread new_thread(workHorse,proxy,client_fd,id); // lack of cache
	// 		id++;
	// 		new_thread.detach();
	// 	}
	// 	catch(exception & e){
	// 		cout<<e.what()<<endl;
	// 		close(listen_fd);
	// 	}
	// }

	return 0;
}