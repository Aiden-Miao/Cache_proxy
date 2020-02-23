#include"proxy.hpp"

int main(){
	Proxy proxy;
	proxy.getAddressInfo();
	proxy.createSocketFd();
	proxy.startListening();//set, bind, listen
	proxy.acceptConnection();//get the socket fd of accept
	proxy.textProxy();
	return 0;
}