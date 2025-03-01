#pragma once

#include <string>
#include <thread>
#include <map>
#include <WinSock2.h>

constexpr int MAX_PLAYERS = 2;

class Server
{
public:
	static Server& Get();

public:
	void beginPlay();
	void endPlay();
	void tick(float elapsedTime);
	void connectClient(SOCKET clientSocket);
	void disconnectClient(SOCKET clientSocket);
	void reserveShutdown(const std::string shutdownReason);
	bool isShutdownReserved();
	bool isShutdownCompleted();
	void replicateGameState();
	void receiveMessageFromClients();
	void messageHandler(SOCKET clientSocket, char* buffer, int bytesReceived);

private:
	SOCKET serverSocket;
	fd_set masterSet;
	int connectedPlayers = 0;
	std::map<int, int> clientSocketToPlayerID;
	std::map<int, int> playerIDToClientSocket;
	std::thread receiveMessageThread;
	bool shutdownReserved = false;
	bool shutdownCompleted = false;
	float shutdownTimer = 0.f;
};
