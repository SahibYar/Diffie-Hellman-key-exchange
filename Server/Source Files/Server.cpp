#include <iostream>
#include <process.h>
#include <random>
#include <conio.h>
#include "ServerClass.h"

int Menu()
{
	int option; //define option variable
	system("cls"); //clear the screen 
	std::cout << "\t\t\t<---::Menu::--->" << std::endl;
	std::cout << "\t\t\t _________________   " << std::endl;
	std::cout << "\t\t\t| 1. Start Server |  " << std::endl;
	std::cout << "\t\t\t| 2. About        |  " << std::endl;
	std::cout << "\t\t\t| 3. Help         |  " << std::endl;
	std::cout << "\t\t\t| 4. Quit         |  " << std::endl;
	std::cout << "\t\t\t|_________________|  " << std::endl;
	std::cout << "\t\t\tOption: ";
	std::cin >> option;  //take the input of option
	std::cin.ignore();

	return option;
}

void CommandHandle(NetworkInit& a);
void CommandHandle(NetworkInit& a)
{
	char command[20] = { 0 };

	std::cin.ignore();

	while (1)
	{
		if (kbhit())
		{
			std::cin.getline(command, 20);
			if (strcmp(command, "X") == 0)
			{
				std::cout << "Host Generating (g^x mod p) and send to client..\n" << std::endl;

				unsigned int gXModP = 0;
				while (gXModP == 0)
				{
					std::random_device dev;
					std::mt19937 rng(dev());
					std::uniform_int_distribution<std::mt19937::result_type> distP(2, 10); // distribution in range
					X = distP(rng);
					gXModP = pow(G, X);
				}

				gXModP = gXModP / P;

				std::string s = std::to_string(gXModP);
				s.append("X");
				strcpy(command, s.c_str());

			}

			//if (SharedKeyGenerated)
			//{
			//	std::string H = K.append(command);
			//	H = cipher::DeffieHellman::SHA1(H);
			//	std::string C = std::string(command).append(H);
			//	encryptedStr = cipher::DeffieHellman::encrypt(std::string(C), std::to_string(PW));
			//	std::cout << "\nEncrypted Msg:\t\t" << encryptedStr << std::endl;
			//}
			//else
			//{
			std::string encryptedStr = cipher::DeffieHellman::encrypt(std::string(command), std::to_string(PW));
			std::cout << "\nEncrypted Msg:\t\t" << encryptedStr << std::endl;
			//}
			send(ClientConn, encryptedStr.c_str(), sizeof(encryptedStr), 0);
		}
		if (!strcmp(command, "quit"))
		{
			a.~NetworkInit();
			break;
		}
	}
}

void RunServer();

void RunServer()
{
	std::fstream file;
	file.open("../UDPServer/Gen.txt", std::ios::in);
	file >> P;
	file >> G;
	file >> PW;
	file.close();

	char IP[16];
	u_short PORT;
	std::thread CommandHandler;

	system("cls");
	std::cout << "P, G, and Password loaded from Gen.txt: \n";
	std::cout << "P= " << P << std::endl;
	std::cout << "G= " << G << std::endl;
	std::cout << "PW= " << PW << std::endl;


	std::cout << "\n\n\t\tEnter the server ip address: ";
	std::cin >> IP;
	std::cout << std::endl;

	std::cout << "\t\tEnter the server port: ";
	std::cin >> PORT;
	std::cout << std::endl;


	NetworkInit com(IP, PORT);

	CommandHandler = std::thread(CommandHandle, std::ref(com));
	com.AcceptConn();
}

void about();

void about()
{
	system("cls");
	std::cout << "Made By: Gordon C" << std::endl;
	std::cout << "UDP Chat Server" << std::endl;
	system("pause");
}

void help();

void help()
{
	system("cls");
	std::cout << "Press 1 to start server" << std::endl;
	std::cout << "Then enter the ip address on which the server should be accessible...it should be your PC's local ip or 127.0.0.1 for localhost" << std::endl;
	std::cout << "After that enter the port on which it should work" << std::endl;
	std::cout << "Now the Server is setup and now clients can connect" << std::endl;
	std::cout << "Check the documentation for more details..." << std::endl;
	system("pause");
}
int main()
{
	do
	{
		switch (Menu())
		{
		case 1:
			RunServer();
			break;

		case 2:
			about();
			break;

		case 3:
			help();
			break;

		case 4:
			exit(0);
			break;
		}
	} while (1);

	return 0;
}
