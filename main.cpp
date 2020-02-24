#include"proxy.hpp"
using namespace std;
int main(){
	Proxy proxy;
	// create a cache
	proxy.getAddressInfo();
	proxy.createSocketFd();
	proxy.startListening();//set, bind, listen
	int listen_fd = proxy.getListenFd();
	cout<<"Start listening on port 4444..."<<endl;
	size_t id = 0;
	signal(SIGPIPE,SIG_IGN);
	while(true){
		proxy.acceptConnection();
		if((int client_fd = proxy.getClientFd())==-1){
			continue;
		}
		try{
			thread new_thread(workHorse,proxy,client_fd,id); // lack of cache
			id++;
			new_thread.detach();
		}
		catch(exception & e){
			cout<<e.what()<<endl;
			close(listen_fd);
		}
	}
	//proxy.acceptConnection();//get the socket fd of accept
	//proxy.textProxy();
	return 0;
}