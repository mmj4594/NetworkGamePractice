#include "SharedData.h"
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Server.h"
#include "Game.h"

#pragma comment(lib, "ws2_32.lib")

LogCategory LogServer("Server");

Server& Server::Get()
{
	static Server instance;
	return instance;
}

void Server::beginPlay()
{
	LOG(LogServer, LogVerbosity::Log, "Server is Started");
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
	LOG(LogServer, LogVerbosity::Log, "Listening for %d Clients", MAX_PLAYERS);

	// Start Message Receiving Thread
	receiveMessageThread = std::thread(&Server::receiveMessageFromClients, this);
}

void Server::endPlay()
{
	// End Game
	Game::Get().endPlay();

	// Disconnect All clients
	const int cachedConnectedPlayers = connectedPlayers;
	for (int i = 1; i <= cachedConnectedPlayers; ++i)
	{
		if (playerIDToClientSocket.find(i) != playerIDToClientSocket.end())
			disconnectClient(playerIDToClientSocket[i]);
	}

	// Close Server Socket
	closesocket(serverSocket);
	WSACleanup();
	
	// Wait for Thread
	if (receiveMessageThread.joinable())
	{
		receiveMessageThread.join();
	}

	LOG(LogServer, LogVerbosity::Log, "Server is Terminated");
	shutdownCompleted = true;
}

void Server::tick(float elapsedTime)
{
	if (isShutdownReserved())
	{
		endPlay();
		return;
	}

	replicateGameState();
}

void Server::connectClient(SOCKET clientSocket)
{
	connectedPlayers++;
	clientSocketToPlayerID[static_cast<int>(clientSocket)] = connectedPlayers;
	playerIDToClientSocket[connectedPlayers] = static_cast<int>(clientSocket);
	FD_SET(clientSocket, &masterSet);
	LOG(LogServer, LogVerbosity::Log, "Accepted Player %d", connectedPlayers);

	// Send Connect Message to Client
	ConnectMessage connectMessage;
	sendMessage(clientSocket, MessageType::Connected, connectMessage);

	// Start Game
	if (connectedPlayers == MAX_PLAYERS)
	{
		LOG(LogServer, LogVerbosity::Log, "%d Players are Connected!", MAX_PLAYERS);
		Game::Get().beginPlay();
	}
}

void Server::disconnectClient(SOCKET clientSocket)
{
	// Send Disconnect Message to Client
	DisconnectMessage disconnectMessage;
	sendMessage(clientSocket, MessageType::Disconnected, disconnectMessage);

	// Close Client Socket
	closesocket(clientSocket);
	FD_CLR(clientSocket, &masterSet);
	connectedPlayers--;

	const int cachedPlayerID = clientSocketToPlayerID[static_cast<int>(clientSocket)];
	playerIDToClientSocket.erase(cachedPlayerID);
	clientSocketToPlayerID.erase(static_cast<int>(clientSocket));
	LOG(LogServer, LogVerbosity::Log, "Disconnected Player %d", cachedPlayerID);
}

void Server::reserveShutdown(const std::string shutdownReason)
{
	shutdownReserved = true;
	LOG(LogServer, LogVerbosity::Log, "Server Shutdown Reserved. Reason: [%s]", shutdownReason.c_str());
}

bool Server::isShutdownReserved()
{
	return shutdownReserved;
}

bool Server::isShutdownCompleted()
{
	return shutdownCompleted;
}

void Server::replicateGameState()
{
	ReplicatedGameState replicatedGameState;
	replicatedGameState.player1Position = Game::Get().player1.getPosition();
	replicatedGameState.player2Position = Game::Get().player2.getPosition();
	replicatedGameState.ballPosition = Game::Get().ball.getPosition();
	replicatedGameState.scorePlayer1 = Game::Get().scorePlayer1;
	replicatedGameState.scorePlayer2 = Game::Get().scorePlayer2;
	replicatedGameState.lastRoundWinnerPlayerID = Game::Get().lastRoundWinnerPlayerID;
	replicatedGameState.currentGameState = Game::Get().currentGameState;
	replicatedGameState.currentRoundState = Game::Get().currentRoundState;

	// Replicate Game State to Clients
	for (int i = 1; i <= connectedPlayers; ++i)
	{
		sendMessage(playerIDToClientSocket[i], MessageType::ReplicateGameState, replicatedGameState);
	}
}

void Server::receiveMessageFromClients()
{
	fd_set readSet;
	FD_ZERO(&masterSet);
	FD_SET(serverSocket, &masterSet);
	int maxSocket = static_cast<int>(serverSocket);
	while (!isShutdownReserved())
	{
		readSet = masterSet;
		select(maxSocket + 1, &readSet, nullptr, nullptr, nullptr);
		for (int i = 0; i <= maxSocket; i++)
		{
			if (shutdownReserved)
				break;

			if (FD_ISSET(i, &readSet))
			{
				// Connect New client
				if (i == serverSocket)
				{
					SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
					if (clientSocket > maxSocket)
						maxSocket = static_cast<int>(clientSocket);
					connectClient(clientSocket);
				}
				// Get Message From Client
				else
				{
					char buffer[MAX_BUFFER_SIZE] = { 0 };
					int bytesReceived = recv(i, buffer, sizeof(buffer), 0);
					if (bytesReceived <= 0)
					{
						disconnectClient(i);
						if(Game::Get().currentGameState != GameStateType::None)
						{
							reserveShutdown("Client Disconnected During the Game");
						}
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
		LOG(LogServer, LogVerbosity::Error, "messageHandler: Invalid Header");
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
			LOG(LogServer, LogVerbosity::Error, "messageHandler: Unhandled Message Received From Client");
		}
	}
}
