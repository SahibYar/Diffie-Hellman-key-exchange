#include "Server.h"
#include <thread>

Server serva;
std::thread readthread, writethread;

void readfunc() {
	serva.UpdateRecv();
}

void writefunc() {
	serva.UpdateSend();
	exit(0);
}

int main() {
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

	serva.CreateServer();
	serva.UpdateRecv();

	std::thread readthread = std::thread(readfunc);
	std::thread writethread = std::thread(writefunc);

	readthread.join();
	writethread.join();

	// Close down Winsock
	WSACleanup();
	return 0;
}
