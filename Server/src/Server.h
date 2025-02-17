#pragma once

#include <thread>
#include <WinSock2.h>

constexpr int MAX_PLAYERS = 2;
constexpr int BUFFER_SIZE = 1024;

class Server
{
public:
	static Server& Get();

public:
	void beginPlay();
	void endPlay();
	void receiveMessageFromClients();

private:
	SOCKET serverSocket;
	SOCKET clientSockets[MAX_PLAYERS];
	int connectedPlayers = 0;
	std::thread receiveMessageThread;
};
