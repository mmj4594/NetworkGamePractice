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
	// Disconnect Clients
	for (std::pair<int, int> Pair : playerIDToClientSocket)
	{
		disconnectClient(Pair.second);
	}

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
	replicatedGameState.scorePlayer1 = Game::Get().scorePlayer1;
	replicatedGameState.scorePlayer2 = Game::Get().scorePlayer2;
	replicatedGameState.currentGameState = Game::Get().currentGameState;
	replicatedGameState.currentRoundState = Game::Get().currentRoundState;

	// Replicate Game State to Clients
	for(int i = 1; i <= connectedPlayers; ++i)
	{
		sendMessage(playerIDToClientSocket[i], MessageType::ReplicateGameState, replicatedGameState);
	}
}

bool Server::shouldClose()
{
	// Player is Disconnected After Game Starts
	if (Game::Get().currentGameState != GameStateType::None && connectedPlayers < MAX_PLAYERS)
		return true;

	// Game is Finished
	if (Game::Get().currentGameState == GameStateType::End)
		return true;

	return false;
}

void Server::connectClient(SOCKET clientSocket)
{
	connectedPlayers++;
	clientSocketToPlayerID[static_cast<int>(clientSocket)] = connectedPlayers;
	playerIDToClientSocket[connectedPlayers] = static_cast<int>(clientSocket);
	FD_SET(clientSocket, &masterSet);

	// Send Connect Message to Client
	ConnectMessage connectMessage;
	sendMessage(clientSocket, MessageType::Connected, connectMessage);

	// Start Game
	if (connectedPlayers == MAX_PLAYERS)
	{
		std::cout << MAX_PLAYERS << " Players are Connected!" << std::endl;
		Game::Get().beginPlay();
	}
}

void Server::disconnectClient(SOCKET clientSocket)
{
	// Send Disconnect Message to Client
	DisconnectMessage disconnectMessage;
	sendMessage(clientSocket, MessageType::Disconnected, disconnectMessage);

	// Close Socket
	closesocket(clientSocket);
	FD_CLR(clientSocket, &masterSet);
	connectedPlayers--;
}

void Server::receiveMessageFromClients()
{
	fd_set readSet;
	FD_ZERO(&masterSet);
	FD_SET(serverSocket, &masterSet);
	int maxSocket = static_cast<int>(serverSocket);
	while (!shouldClose())
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
					std::cout << "Accepted Player " << connectedPlayers << std::endl;
					if (clientSocket > maxSocket)
						maxSocket = static_cast<int>(clientSocket);
					connectClient(clientSocket);
				}
				// Get Message From Client
				else
				{
					char buffer[BUFFER_SIZE] = { 0 };
					int bytesReceived = recv(i, buffer, sizeof(buffer), 0);
					if (bytesReceived <= 0)
					{
						std::cout << "Disconnect Client!" << std::endl;
						disconnectClient(i);
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
			PlayerInput playerInput;
			deserialize(buffer + sizeof(MessageHeader), playerInput);
			Game::Get().onKey(clientSocketToPlayerID[static_cast<int>(clientSocket)], playerInput.key, playerInput.scancode, playerInput.action, playerInput.mods);
		}
		break;
		default:
		{
			std::cout << "Unhandled Message Received From Client!" << std::endl;
		}
	}
}
