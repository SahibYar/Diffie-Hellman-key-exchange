#ifndef _CHAT_SERVER
#define _CHAT_SERVER

#include <iostream>
#include <cstdlib>
#include <vector>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

#define MAXDATASIZE 1024
#define PORT "8000" //port we are listening to
#define LOCALHOST "127.0.0.1"

class Server {
public:
	Server():sockfd(0), isServerRunning(true){ }
	~Server() { }

	void CreateServer();
	void UpdateRecv();
	void UpdateSend();
	void HandleMessage();
	void BroadcastMessage(std::string _message, SOCKADDR* _destination);
	void BroadcastMessageToAll(std::string _message);
	void CloseServer();

	bool isServerRunning;
private:
	SOCKET sockfd;
	SOCKADDR_STORAGE remoteaddr;		//client addresses
	socklen_t remoteaddrlen;
	char buf[MAXDATASIZE];				//buffer for client data
	int nbytes;
	char remoteIP[INET6_ADDRSTRLEN];

	int client, otherClients, returnedValue;

	ADDRINFO hints, *addressInfo, *point;

	std::vector<SOCKADDR> clientsVec;

	bool CompareClients(SOCKADDR* c_1, SOCKADDR* c_2);
};

#endif //_CHAT_SERVER
