#include <iostream>
#include <thread>
#include "ClassClient.hpp"


using namespace std;

int main()
{
	char ip[15];                   //variable to store the ip address
	u_short port;
	thread recv;

	cout << "Client" << endl;
	cout << "Enter the ip address of the server: ";
	cin >> ip;

	cout << "Enter the port of the server: ";
	cin >> port;

	ClientNetworkInit cli(ip, port);         //construct the ClientNetworkInit object
	cli.ConnectServer();                    //connect to the specified server

	ClientNetworkComInit(ip, port);         //setup to enter your id and the id of the user you want to chat with

	recv = thread(ClientNetworkComRecv);    //start the reciever on  a separate thread
	recv.detach();

	ClientNetworkComSend();                //run the sender in the main thread
	
	return 0;
}
