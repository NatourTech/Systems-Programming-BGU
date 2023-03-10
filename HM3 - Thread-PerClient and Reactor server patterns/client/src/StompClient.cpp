#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"
#include <thread>

using namespace std;

int main(int argc, char *argv[]) {
	StompProtocol stompProtocol;
	ConnectionHandler *connectionHandler = new ConnectionHandler("",0);
	while(1){
	while(!stompProtocol.connected){
		cout<<"Try to login..\n"<<endl;
		const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);
		std::vector<std::string> cmd = stompProtocol.StompProtocol::Splitter(line,' ');
		cmd = stompProtocol.StompProtocol::Splitter(cmd[1],':');
		string host = cmd[0];
		int port = stoi(cmd[1]);
		delete connectionHandler;
		connectionHandler = new ConnectionHandler(host,port);
		if(!connectionHandler->connect())
		{
			std::cout<<"Could not connect to server..."<<endl;
			std::cout<<"please try again..."<<std::endl;
		}
		else
		{
			if(!connectionHandler->sendLine(stompProtocol.Proccess(line)[0])){
				std::cout<<"Can't Do Dat (sending line)"<<std::endl;
				stompProtocol.connected = false;
				connectionHandler->close();
				break;
			}
			std::string reply;
        	if (!connectionHandler->getLine(reply)) {
          			std::cout << "Disconnected. Exiting...\n" << std::endl;
					stompProtocol.connected = false;
			}
			else
			{
				if(!stompProtocol.Proccess_reply(reply))
					{
						cout<<"\n"<<"Disconnecting...."<<endl;
						connectionHandler->close();
						stompProtocol.connected = false;
					}
			}
		}
	}
			std::thread writer([&]() {
				while(stompProtocol.connected){
					const short bufsize = 1024;
					char buf[bufsize];
					std::cin.getline(buf, bufsize);	
					std::string line(buf);
					vector<std::string> toSend = stompProtocol.Proccess(line);
					if(stompProtocol.connected){
					if(toSend.size() != 0){
					for(string &curr : toSend){
					if(curr.length() != 0 && !connectionHandler->sendLine(curr)){
						cout<<"told you cant do dat"<<endl;
						cout<<"Disconnecting"<<endl;
						stompProtocol.connected = false;
					}
					}
					}
					}
				}						
				});


				while(stompProtocol.connected){
				std::string reply;
        		if (!connectionHandler->getLine(reply)) {
          			std::cout << "Disconnected. Exiting...\n" << std::endl;
					stompProtocol.connected = false;
					}
				else{
					if(!stompProtocol.Proccess_reply(reply))
					{
						cout<<"\n"<<"Disconnecting...."<<endl;
						connectionHandler->close();
						stompProtocol.connected = false;
					}
				}
				}
	writer.join();
	}
	delete connectionHandler;
	return 0;
}