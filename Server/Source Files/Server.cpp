#include "Server.h"
#include "../DeffieHellman/Cipher.h"

void* get_in_addr(SOCKADDR* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((SOCKADDR_IN*)sa)->sin_addr);
	}
	return &(((SOCKADDR_IN6*)sa)->sin6_addr);
}

void Server::CreateServer() {
	//get us a socket and bind it
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = IPPROTO_UDP;	//use my IP
	hints.ai_protocol = 0;

	if ((returnedValue = getaddrinfo(LOCALHOST, PORT, &hints, &addressInfo)) != 0)
	{
		printf("selectserver: %ls\n", gai_strerror(returnedValue));
		exit(1);
	}

	for (point = addressInfo; point != NULL; point = point->ai_next) 
	{
		sockfd = socket(point->ai_family, point->ai_socktype, point->ai_protocol);
		if (sockfd == -1) {
			printf("listener: socket");
			continue;
		}

		if (bind(sockfd, point->ai_addr, point->ai_addrlen) == -1) {
			closesocket(sockfd);
			printf("listener: bind");
			continue;
		}
		break;
	}
	//if we got here, it means we didn't get bound
	if (point == NULL) 
	{
		printf("selectserver: failed to bind\n");
		exit(2);
	}
	freeaddrinfo(addressInfo); //all done with this
	remoteaddrlen = sizeof(remoteaddr);

	printf("We are NOW running!\n");
}

void Server::UpdateRecv() 
{
	while (isServerRunning) 
	{

		if ((nbytes = recvfrom(sockfd, buf, MAXDATASIZE - 1, 0, (SOCKADDR*)&remoteaddr, &remoteaddrlen)) == -1) 
		{
			return;
		}
		if (nbytes > 0) {
			HandleMessage();
		}
	}
}

void Server::UpdateSend() {
	std::string message;
	while (isServerRunning) {
		getline(std::cin, message);
		if (strcmp(message.c_str(), "/end") == 0) 
		{
			//Send end message to all users
			BroadcastMessageToAll(std::string("4:Serva:Server has been shut down!"));
			//Shut the server down
			isServerRunning = false;
			return;
		}
		else {
			BroadcastMessageToAll(std::string("1:Serva:") + message);
		}
	}
}

void Server::HandleMessage() {
	//handle data from a client
	//decrypt buf
	std::string decryptedStr = cipher::DeffieHellman::decrypt(std::string(buf), -3);
	char code = decryptedStr[0];

	if (code == '0') //client is logging on
	{	
		printf("Client joined.\n");

		//Add client's address to the vector
		clientsVec.push_back(*(SOCKADDR*)&remoteaddr);
		
		printf("clients size: %i\n", int(clientsVec.size()));
	}
	else if (code == '1')  //client is sending a message
	{
	   //printf("listener: packet is %d bytes long\n", nbytes);
		decryptedStr[nbytes] = '\0';
	
		printf("(%s): %s\n", inet_ntop(remoteaddr.ss_family, get_in_addr((SOCKADDR*)&remoteaddr), remoteIP, sizeof(remoteIP)), decryptedStr.c_str());
		//send message to all other clients
		BroadcastMessageToAll(decryptedStr);
	}
	else if (code == '2')  //client is logging out
	{
		printf("Client left.\n");
		for (size_t i = 0; i < clientsVec.size(); i++) {
			if (CompareClients(&clientsVec[i], (SOCKADDR*)&remoteaddr)) {
				printf("Going to erase\n");
				clientsVec.erase(clientsVec.begin() + i);
				break;
			}
		}
		printf("clients size: %i\n", int(clientsVec.size()));
		//shut down server if everyone leaves
		if (clientsVec.size() == 0) {
			return;
		}
	}
}

void Server::BroadcastMessage(std::string _message, SOCKADDR* _destination) {
	remoteaddrlen = sizeof(*_destination);
	sendto(sockfd, _message.c_str(), _message.length(), 0, _destination, remoteaddrlen);
}

void Server::BroadcastMessageToAll(std::string _message) {
	for (size_t i = 0; i < clientsVec.size(); i++) {
		//disable the if check if you want to test on localhost
		// if(!CompareClients(&clientsVec[i], (struct sockaddr *)&remoteaddr)){
		BroadcastMessage(_message, &clientsVec[i]);
		// }
	}
}

bool Server::CompareClients(SOCKADDR* c_1, SOCKADDR* c_2) {
	return (inet_ntop(remoteaddr.ss_family, get_in_addr(c_1), remoteIP, sizeof remoteIP) ==
		inet_ntop(remoteaddr.ss_family, get_in_addr(c_2), remoteIP, sizeof remoteIP));
}

void Server::CloseServer() {
	closesocket(sockfd);
}
