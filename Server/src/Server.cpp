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
	ReplicateGameStateMessage message;
	ReplicatedGameState replicatedGameState;
	replicatedGameState.player1Position = Game::Get().player1.getPosition();
	replicatedGameState.player2Position = Game::Get().player2.getPosition();
	replicatedGameState.ballPosition = Game::Get().ball.getPosition();
	replicatedGameState.scorePlayer1 = Game::Get().scorePlayer1;
	replicatedGameState.scorePlayer2 = Game::Get().scorePlayer2;
	replicatedGameState.currentGameState = Game::Get().currentGameState;
	replicatedGameState.currentRoundState = Game::Get().currentRoundState;
	message.header.type = MessageType::ReplicateGameState;
	message.header.size = sizeof(ReplicatedGameState);
	message.gameState = replicatedGameState;

	char buffer[sizeof(message)];
	serialize(message, buffer);

	// Replicate Game State to Clients
	for(int i = 1; i <= connectedPlayers; ++i)
	{
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
					std::cout << "Accepted Player " << connectedPlayers << std::endl;

					if (connectedPlayers == MAX_PLAYERS)
					{
						std::cout << MAX_PLAYERS << " Players are Connected!" << std::endl;
						Game::Get().beginPlay();
					}
				}
				// Get Message From Client
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
						messageHandler(i, buffer, bytesReceived);
					}
				}
			}
		}
	}
}

void Server::messageHandler(SOCKET clientSocket, char* buffer, int bytesReceived)
{
	MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer);
	if (header == nullptr)
	{
		std::cout << "Invalid Header!" << std::endl;
		return;
	}

	switch (header->type)
	{
		case MessageType::PlayerInput:
		{
			PlayerInputMessage message;
			deserialize(buffer, message);
			Game::Get().onKey(clientSocketToPlayerID[static_cast<int>(clientSocket)],
				message.playerInput.key, message.playerInput.scancode, message.playerInput.action, message.playerInput.mods);
		}
		break;
		default:
		{
			std::cout << "Unhandled Message Received From Client!" << std::endl;
		}
	}
}
