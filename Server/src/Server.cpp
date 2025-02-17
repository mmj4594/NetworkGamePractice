#include <iostream>
#include <string>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Server.h"

#pragma comment(lib, "ws2_32.lib")

Server& Server::Get()
{
	static Server instance;
	return instance;
}

void Server::beginPlay()
{
	WSADATA wsaData;
	struct sockaddr_in serverAddr, clientAddr;
	int clientAddrSize = sizeof(clientAddr);

	// Windows Socket API Startup
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Server Socket(IPv4, TCP)
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(9000);
	bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	// Ready to connect with MAX_PLAYERS Clients
	connectedPlayers = 0;
	listen(serverSocket, MAX_PLAYERS);
	std::cout << "Listening for " << MAX_PLAYERS << " Clients" << std::endl;

	// Start Message Receiving Thread
	receiveMessageThread = std::thread(&Server::receiveMessageFromClients, this);
}

void Server::endPlay()
{
	// Close Sockets
	closesocket(serverSocket);
	WSACleanup();
	
	// Wait for Thread
	if (receiveMessageThread.joinable())
	{
		receiveMessageThread.join();
	}
}

void Server::receiveMessageFromClients()
{
	fd_set masterSet, readSet;
	FD_ZERO(&masterSet);
	FD_SET(serverSocket, &masterSet);
	int maxSocket = static_cast<int>(serverSocket);
	while (true)
	{
		readSet = masterSet;
		select(maxSocket + 1, &readSet, nullptr, nullptr, nullptr);
		for (int i = 0; i <= maxSocket; i++)
		{
			if (FD_ISSET(i, &readSet))
			{
				// Connect New client
				if (i == serverSocket)
				{
					SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
					clientSockets[connectedPlayers] = clientSocket;
					connectedPlayers++;
					FD_SET(clientSocket, &masterSet);
					if (clientSocket > maxSocket)
						maxSocket = static_cast<int>(clientSocket);
					std::cout << "Accepted Client " << connectedPlayers << std::endl;

					if (connectedPlayers == MAX_PLAYERS)
					{
						std::cout << MAX_PLAYERS << " Players are Connected!" << std::endl;
					}
				}
				// Get Data From Client
				else
				{
					char buffer[BUFFER_SIZE] = { 0 };
					int bytesReceived = recv(i, buffer, sizeof(buffer), 0);
					if (bytesReceived <= 0)
					{
						std::cout << "Disconnect Client!" << std::endl;
						closesocket(i);
						FD_CLR(i, &masterSet);
					}
					else
					{
						std::cout << "Message from Client " << i << ": " << buffer << std::endl;
						const char* message = "Hello, Client!";
						send(i, message, static_cast<int>(strlen(message)), 0);
						std::cout << "Send Message to Client " << i << ": " << message << std::endl;
					}
				}
			}
		}
	}
}
