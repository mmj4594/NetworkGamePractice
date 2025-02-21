#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "SharedData.h"
#include "Server.h"
#include "Game.h"

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

void Server::tick(float elapsedTime)
{
	ReplicatedGameState replicatedGameState;
	replicatedGameState.player1Position = Game::Get().player1.getPosition();
	replicatedGameState.player2Position = Game::Get().player2.getPosition();
	replicatedGameState.ballPosition = Game::Get().ball.getPosition();

	// Send Game State to Clients
	for(int i = 1; i<=connectedPlayers; ++i)
	{
		char buffer[sizeof(ReplicatedGameState)];
		serialize(replicatedGameState, buffer);
		send(playerIDToClientSocket[i], buffer, static_cast<int>(sizeof(buffer)), 0);
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
					connectedPlayers++;
					clientSocketToPlayerID[static_cast<int>(clientSocket)] = connectedPlayers;
					playerIDToClientSocket[connectedPlayers] = static_cast<int>(clientSocket);
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
						PlayerInput playerInput;
						deserialize(buffer, playerInput);
						std::cout << "Player " << clientSocketToPlayerID[i] << " Sent Input! "
							<< playerInput.key << ", " << playerInput.scancode << ", " << playerInput.action << ", " << playerInput.mods << std::endl;
						Game::Get().onKey(clientSocketToPlayerID[i], playerInput.key, playerInput.scancode, playerInput.action, playerInput.mods);
					}
				}
			}
		}
	}
}
