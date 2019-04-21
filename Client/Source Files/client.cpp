#include "Client.h"
#include <sstream>
#include <any>
#include <vector>
#include "../DeffieHellman/Cipher.h"

void* get_in_addr(SOCKADDR* sa)
{
	if (sa->sa_family == AF_INET)
		return &(((SOCKADDR_IN*)sa)->sin_addr);

	return &(((SOCKADDR_IN6*)sa)->sin6_addr);
}

void Client::InitClient() 
{
	std::string hostname;

	//Get hostname
	std::cout << "Specify the hostname/IP you would like to connect to: ";
	getline(std::cin, hostname);

	ConnectToServer(hostname.c_str());
}

void Client::ConnectToServer(const char* _hostname) {
	//Connect to hostname
	char s[INET6_ADDRSTRLEN];
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(_hostname, PORT, &hints, &servinfo)) != 0) 
	{
		printf("getaddrinfo: %ls\n", gai_strerror(rv));
		return;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		//fcntl(sockfd, F_SETFL, O_NONBLOCK);
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to bind socket\n");
		return;
	}

	inet_ntop(p->ai_family, get_in_addr((SOCKADDR*)p->ai_addr), s, sizeof(s));
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); //all done with this structure

	//Get user's name
	std::cout << "Welcome to the Chat server. What is your name? \n";
	getline(std::cin, name);

	//let server know that we're here
	std::string joinMessage = std::string("0 ") + name + " joined.";
	numbytes = sendto(sockfd, joinMessage.c_str(), joinMessage.length(), 0, p->ai_addr, p->ai_addrlen);

	if (numbytes == -1)
	{
		perror("talker: sendto");
		exit(1);
	}

	clientRunning = true;

	printf("Welcome %s!\n", name.c_str());
}

void Client::SendUpdate() {
	std::string message;
	while (clientRunning) 
	{
		getline(std::cin, message);
		if (strcmp(message.c_str(), "/logout") == 0) 
		{
			//let server know that we're gone like the wind
			std::string leaveMessage = name + " left.";
			SendEncryptedMessage('2', leaveMessage);
			printf("Logged out\n");
			clientRunning = false;
			CloseConnection();
			return;
		}
		SendEncryptedMessage('1', message);
	}
}

void Client::ReceiveUpdate() {
	while (clientRunning) 
	{
		int size = p->ai_addrlen;
		numbytes = recvfrom(sockfd, buf, MAXDATASIZE - 1, 0, p->ai_addr, &size);
		p->ai_addrlen = size;

		if (numbytes != -1)
		{
			ReceiveMessage(std::string(buf));
		}
	}
}

void Client::SendEncryptedMessage(char _cmd, std::string _message) {
	//encrypt then send to server
	std::string fullMessage = cipher::DeffieHellman::encrypt(std::string(1, _cmd) + ":" + name + ":" + _message, -3);
	if ((numbytes = sendto(sockfd, fullMessage.c_str(), fullMessage.length(), 0,
		p->ai_addr, p->ai_addrlen)) == -1) {
		perror("talker: sendto");
		exit(1);
	}
}

void Client::ReceiveMessage(std::string _incomingMessage) {
	//Making sure there isn't a newline at the end
	_incomingMessage[numbytes] = '\0';
	//we will be splitting the incoming message by delimiter
	std::istringstream iss(_incomingMessage);
	std::vector<std::string> linesSplit;
	while (std::getline(iss, _incomingMessage, ':')) {
		linesSplit.push_back(_incomingMessage);
	}
	//first string should be command
	//second should be name
	//third should be actual message
	if (linesSplit.size() >= 3) {
		if (linesSplit[1] == "4") {
			//client exit
			clientRunning = false;
		}
	}
	if (linesSplit.size() >= 3) {
		printf("\r(%s): %s\n", linesSplit[1].c_str(), linesSplit[2].c_str());
	}
}

void Client::CloseConnection() {
	closesocket(sockfd);
}
