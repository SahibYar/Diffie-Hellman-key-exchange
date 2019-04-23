#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <random>
#include <fstream>
#include "../DeffieHellman/Cipher.h"
using namespace std;

int G = 0;
int P = 0;
int PW = 0;
unsigned int Y = 0;
unsigned int X = 0;
string K = "";
bool SharedKeyGenerated = false;

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

SOCKET ServerCom = INVALID_SOCKET;
int ClientAddressSize;
SOCKADDR_IN ClientAddress;

void ClientNetworkComInit(char ip[], u_short port)
{
	string cMessageRecv;
	char cMessage[512] = " ";
	int cstate = 0;

	do
	{
		cstate = recv(ServerCom, cMessage, 512, 0);
	} while (cstate <= 0);
	cMessageRecv = cMessage;
	cout << "Hello: " << cMessageRecv << "(from client)" << endl;
	cstate = 0;
	strcpy(cMessage, " ");
	cin.ignore();

	do
	{
		cstate = send(ServerCom, cMessage, 512, 0);
	} while (cstate <= 0);
	cstate = 0;

	system("cls");

	cout << "\t\tChat Client" << endl;
	cout << "Connected to: " << ip << "@" << port << endl;
	std::cout << "Press 'Y' to generate random y" << std::endl;
	cout << "To quit chat type EXIT" << endl << endl;

	std::fstream file;
	file.open("../UDPServer/Gen.txt", std::ios::in);
	file >> P;
	file >> G;
	file >> PW;
	file.close();

}

//call this in a thread
void ClientNetworkComSend()
{
	char MessageSend[512] = " ";
	int mstate = 0;

	while (strcmp(MessageSend, "EXIT")) 
	{
		cin.getline(MessageSend, 512);

		if (strcmp(MessageSend, "Y") == 0)
		{
			std::cout << "Client Generating (g^y mod p) and send to Host..\n" << std::endl;

			unsigned int gYModP = 0;
			while (gYModP == 0)
			{
				std::random_device dev;
				std::mt19937 rng(dev());
				std::uniform_int_distribution<std::mt19937::result_type> distP(2, 10); // distribution in range
				Y = distP(rng);
				gYModP = pow(G, Y);
			}
			gYModP = gYModP / P;

			std::string s = std::to_string(gYModP);
			s.append("Y");
			strcpy(MessageSend, s.c_str());
		}

		//if (SharedKeyGenerated)
		//{
		//	string H = K.append(MessageSend);
		//	H = cipher::DeffieHellman::SHA1(H);
		//	string C = std::string(MessageSend).append(H);
		//	encryptedStr = cipher::DeffieHellman::encrypt(std::string(C), std::to_string(PW));
		//	std::cout << "\nEncrypted Msg:\t\t" << encryptedStr << std::endl;
		//}
		//else
		//{
		std::string encryptedStr = cipher::DeffieHellman::encrypt(std::string(MessageSend), std::to_string(PW));
		std::cout << "\nEncrypted Msg:\t\t" << encryptedStr << std::endl;
		//}
		do
		{
			mstate = send(ServerCom, encryptedStr.c_str(), encryptedStr.length(), 0);
		} while (mstate <= 0);
	}
	exit(0);
}

// call this is separate thread
void ClientNetworkComRecv()
{
	string cMessageRecv = "";
	char cMessageRcv[512] = { 0 };
	int flag = 0;

	while (strcmp(cMessageRcv, "EXIT"))
	{
		do
		{
			flag = recv(ServerCom, cMessageRcv, 512, 0);
		} while (flag <= 0);

		cMessageRecv = cMessageRcv;
		std::string decryptedStr = cipher::DeffieHellman::decrypt(cMessageRecv, std::to_string(PW));
		if (decryptedStr[decryptedStr.length() - 1] == 'X')
		{
			//received value is g^x / p
			decryptedStr = decryptedStr.substr(0, decryptedStr.length() - 1);
			unsigned int gxp = atoi(decryptedStr.c_str());
			gxp = gxp * P;
			X = log10(gxp);
			cout << "X = " << X << endl;

			//while (Y == 0)
			//{
			//	std::random_device dev;
			//	std::mt19937 rng(dev());
			//	std::uniform_int_distribution<std::mt19937::result_type> distP(2, 10); // distribution in range
			//	Y = distP(rng);
			//}

			//std::cout << "Calculating shared key..." << std::endl;
			//K = std::to_string(pow(G, (X*Y)) / P);
			//std::cout << "K=" << K << std::endl;
			//SharedKeyGenerated = true;
		}

		cout << "\t\t\t\t\t\t" << decryptedStr << endl;
	}
	exit(0);
}


class ClientNetworkInit
{
private:
	WSAData ClientNetworkData;
	char message[512];

public:
	ClientNetworkInit(char sIP[], u_short sPORT)
	{
		cout << "Intializing Winsock API";

		for (int i = 0; i < 15; i++)
		{
			cout << ".";
			Sleep(100);
		}
		if (!WSAStartup(MAKEWORD(2, 1), (WSAData*)&ClientNetworkData))
		{
			cout << "done" << endl;
		}
		else
		{
			cout << "Error starting Winsock API" << endl;
			cout << "Shutting down the server" << endl;
			closesocket(ServerCom);
			WSACleanup();
			exit(0);
		}

		ClientAddress.sin_addr.s_addr = inet_addr(sIP);
		ClientAddress.sin_port = htons(sPORT);
		ClientAddress.sin_family = AF_INET;

		ClientAddressSize = sizeof(ClientAddress);
		ServerCom = socket(AF_INET, SOCK_STREAM, 0);

		if (ServerCom == INVALID_SOCKET)
		{
			cout << "Error initialising socket" << endl;
			closesocket(ServerCom);
			WSACleanup();
			exit(0);
		}
		else
		{
			cout << "Socket Intialised" << endl;
		}
	}
	void ConnectServer()
	{
		int val;
		val = connect(ServerCom, (SOCKADDR*)&ClientAddress, ClientAddressSize);
		if (!val)
		{
			cout << "Connected to server" << endl;
		}

		else
		{
			cout << "Unable to connect to the server" << endl;
			cout << "Closing chat client" << endl;
			WSACleanup();
			exit(0);
		}
	}
};
