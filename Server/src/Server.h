#pragma once

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
	void receiveMessageFromClients();

private:
	SOCKET serverSocket;
	int connectedPlayers = 0;
	std::map<int, int> clientSocketToPlayerID;
	std::thread receiveMessageThread;
};
