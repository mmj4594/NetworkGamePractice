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
	void tick(float elapsedTime);
	void receiveMessageFromClients();
	void messageHandler(SOCKET clientSocket, char* buffer, int bytesReceived);
	template <typename T>
	void sendMessage(SOCKET clientSocket, MessageType type, const T& message)
	{
		int dataSize = sizeof(T);
		int totalSize = sizeof(MessageHeader) + dataSize;

		// Fill Header
		char* buffer = new char[totalSize];
		MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer);
		header->type = type;
		header->size = dataSize;

		// Serialize and Send Message
		serialize(message, buffer + sizeof(MessageHeader));
		send(clientSocket, buffer, totalSize, 0);

		delete[] buffer;
	}
	int getConnectedPlayers() const {return connectedPlayers;}

private:
	SOCKET serverSocket;
	int connectedPlayers = 0;
	std::map<int, int> clientSocketToPlayerID;
	std::map<int, int> playerIDToClientSocket;
	std::thread receiveMessageThread;
};
