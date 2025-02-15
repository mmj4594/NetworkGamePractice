#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "GameMode_Online.h"

void GameMode_Online::beginPlay()
{
	// Connnect to Server
	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in serverAddr;

	// Windows Socket API Startup
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Cretae Client Socket(IPv4, TCP) and Set Target Server Address
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	const char* serverIPAddr = "127.0.0.1";
	inet_pton(AF_INET, serverIPAddr, &serverAddr.sin_addr);

	// Try to Connect to Server
	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cerr << "Failed to Connect to Server!" << std::endl;
		return;
	}
	std::cout << "Connected to Server!" << std::endl;

	// Send Message to Server
	const char* message = "Hello, Server!";
	send(clientSocket, message, static_cast<int>(strlen(message)), 0);

	// Receive Message from Server
	char buffer[1024] = { 0 };
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message from Server: " << buffer << std::endl;

	// Close Sockets
	closesocket(clientSocket);
	WSACleanup();
}

void GameMode_Online::endPlay()
{
}

void GameMode_Online::tick(float elapsedTime)
{
}

void GameMode_Online::renderFrame(float elapsedTime)
{
}

void GameMode_Online::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}
