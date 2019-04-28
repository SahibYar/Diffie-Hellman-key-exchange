#include <iostream>
//#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <thread>
#include <fstream>
#include <string>
#include <memory>
#include "../DeffieHellman/Cipher.h"

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

int ClientStore = 0;

int G = 0;
int P = 0;
int PW = 0;
unsigned int Y = 0;
unsigned int X = 0;
std::string K = "";
bool SharedKeyGenerated = false;

SOCKET ClientConn(0);

void CreateClientThread();
void NetworkCom();
void NetworkCom()
{
	int flag = 0;
	char datatemp[512] = " ";
	char msgtemp[512] = "";
	do
	{
		flag = recv(ClientConn, datatemp, 512, 0);
	} while (flag <= 0);
	flag = 0;

	while (ClientConn != INVALID_SOCKET)
	{
		flag = recv(ClientConn, msgtemp, 512, 0);
		if (flag > 0)
		{
			std::string decryptedStr = "";
			decryptedStr = cipher::DeffieHellman::decrypt(std::string(msgtemp), std::to_string(PW));
			if (decryptedStr[decryptedStr.length() - 1] == 'Y')
			{
				//received value is g^y / p
				decryptedStr = decryptedStr.substr(0, decryptedStr.length() - 1);
				unsigned int gyp = atoi(decryptedStr.c_str());
				gyp = gyp * P;
				Y = log10(gyp);
				std::cout << "Y = " << Y << std::endl;

				//while (X == 0)
				//{
				//	std::random_device dev;
				//	std::mt19937 rng(dev());
				//	std::uniform_int_distribution<std::mt19937::result_type> distP(2, 10); // distribution in range
				//	X = distP(rng);
				//}

				//std::cout << "Calculating shared key..." << std::endl;
				//K = std::to_string(pow(G,(X*Y)) / P);
				//std::cout << "K=" << K << std::endl;
				//SharedKeyGenerated = true;
			}
			std::cout << "\nDecrypted Msg:\t\t" << decryptedStr << std::endl;
		}
		flag = 0;

		if (!strcmp(msgtemp, "EXIT"))
		{
			closesocket(ClientConn);
			std::cout << "Client has disconnected" << std::endl;
			break;
		}
	}

}

void CreateClientThread()
{
	std::thread HandleClient = std::thread(NetworkCom);
	HandleClient.detach();
}

///////////////////////////////////////// Class NetworkInit ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NetworkInit
{
private:
	//declarations of winsock library objects necessary for the sockets parts to run
	WSAData NetworkData;
	SOCKET ListenSocket;
	SOCKADDR_IN Address;
	int AddressSize;

public:
	NetworkInit(); // default constructor

	NetworkInit(char IP[], u_short PORT) // argumented constructor
	{
		//---------------------------------------Initailize Winsock API----------------------------------------------------------------------------------------------------------------------------
		//process to start the winsock api
		std::cout << "Intializing Winsock API";
		for (int i = 0; i < 17; i++)
		{
			std::cout << ".";
			Sleep(100);
		}
		if (WSAStartup(MAKEWORD(2, 2), &NetworkData) == 0)
		{
			std::cout << "done" << std::endl;
		}
		else
		{
			std::cout << "Error starting Winsock API" << std::endl;
			std::cout << "Shutting down the server" << std::endl;
			closesocket(ListenSocket);
			WSACleanup();
			exit(0);
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		Address.sin_addr.s_addr = inet_addr(IP);
		Address.sin_port = htons(PORT);
		Address.sin_family = AF_INET;

		//---------------------------------------Binding Listening Socket--------------------------------------------------------------------------------------------------------------------------		
				//process of binding the socket

		std::cout << "Binding socket";
		for (int i = 0; i < 26; i++)
		{
			std::cout << ".";
			Sleep(50);
		}
		ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (bind(ListenSocket, (SOCKADDR*)&Address, sizeof(Address)) == 0)
		{
			std::cout << "done" << std::endl;
		}
		else
		{
			std::cout << "Error Binding the port" << std::endl;
			std::cout << "Shutting down the server" << std::endl;
			closesocket(ListenSocket);
			WSACleanup();
			exit(0);
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


		//---------------------------------------Intialize listening socket------------------------------------------------------------------------------------------------------------------------		        
				//initialize the listening socket

		std::cout << "Initializing listening socket";
		for (int i = 0; i < 11; i++)
		{
			std::cout << ".";
			Sleep(100);
		}
		if (!listen(ListenSocket, SOMAXCONN))
		{
			std::cout << "done" << std::endl;
		}
		else
		{
			std::cout << "Error in starting listening socket" << std::endl;
			std::cout << "Shutting down the server" << std::endl;
			closesocket(ListenSocket);
			WSACleanup();
			exit(0);
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------		

		AddressSize = sizeof(Address);
		ClientConn = socket(AF_INET, SOCK_STREAM, 0);

		if (ClientConn == INVALID_SOCKET)
		{
			std::cout << "Error in initialising  listening socket " << std::endl;
		}
		else
		{
			std::cout << "Socket intialized " << std::endl;
		}

		system("pause");
		system("cls");

		std::cout << "\t\tUDP Chat Server" << std::endl; // display of server information
		std::cout << "\t\t----------------------------------------" << std::endl;
		std::cout << "\t\t IP Address :" << IP << std::endl;
		std::cout << "\t\t Port Number:" << PORT << std::endl;
		std::cout << "\t\t----------------------------------------" << std::endl;
	}
	//--------------------------------------------Intitalize all client handler sockets--------------------------------------------------------------------------------------------------------		

	void AcceptConn()
	{
		for (;;)
		{
			if ((ClientConn = accept(ListenSocket, (SOCKADDR*)&Address, &AddressSize)))
			{
				std::cout << "Client connected: Press 'X' to generate random x" << std::endl;
				CreateClientThread(); // call function to create new client managing thread
				send(ClientConn, "Hello from Server", sizeof("Hello from Server"), 0); // send necessary instructions to the client
				ClientStore++;		  	//increment the client counter
			}
		}
	}

	~NetworkInit()
	{
		// destructor -- shutdown the winsock api, delete the socket object
		std::cout << "Closing socket " << std::endl;
		closesocket(ClientConn);
		std::cout << "Shutting down Winsock API" << std::endl;
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
