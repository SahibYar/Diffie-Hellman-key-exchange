#ifndef _CHAT_CLIENT
#define _CHAT_CLIENT

#include <iostream>
#include <WS2tcpip.h>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

#define MAXDATASIZE 1024
#define PORT "8000"			//port we are connecting to

class Client {
public:
	Client(std::string n = "Anon") :name(n) {};
	~Client() {};

	void InitClient();
	void ConnectToServer(const char* _hostname);
	void SendUpdate();
	void ReceiveUpdate();
	void SendEncryptedMessage(char _cmd, std::string _message);
	void ReceiveMessage(std::string _incomingMessage);
	void CloseConnection();

	bool clientRunning;
private:
	std::string name;
	int sockfd;
	ADDRINFO hints, *servinfo, *p;
	SOCKADDR_STORAGE remoteaddr;		//server address
	socklen_t remoteaddrlen;

	int numbytes;
	char buf[MAXDATASIZE];
	int rv;
};

#endif //_CHAT_CLIENT
