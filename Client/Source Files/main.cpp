#include "Client.h"
#include <thread>

Client client;
std::thread readthread, writethread;

void readfunc() {
	client.ReceiveUpdate();
}

void writefunc() {
	client.SendUpdate();
}

int main(int argc, char *argv[]) {
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		std::cout << "Can't start Winsock! " << wsOk;
		return -1;
	}

	if (argc >= 2) 
	{
		if (strcmp(argv[1], "--help") == 0) 
		{
			printf("usage: enter hostname\n");
			return 0;
		}
		client.ConnectToServer(argv[1]);
	}
	else 
	{
		client.InitClient();
	}

	readthread = std::thread(readfunc);
	writethread = std::thread(writefunc);

	readthread.join();
	writethread.join();

	// Close down Winsock
	WSACleanup();

	return 0;
}
