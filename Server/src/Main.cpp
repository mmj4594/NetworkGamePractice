#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Game.h"

#pragma comment(lib, "ws2_32.lib")

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	// Connect With Client
	{
		WSADATA wsaData;
		SOCKET serverSocket, clientSocket;
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

		// Wait for 1 Client
		listen(serverSocket, 1);
		std::cout << "Waiting for Client" << std::endl;

		// Accept Client
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		std::cout << "Accepted Client!" << std::endl;

		// Receive Message from Client
		char buffer[1024] = { 0 };
		recv(clientSocket, buffer, sizeof(buffer), 0);
		std::cout << "Message from Client: " << buffer << std::endl;

		// Send Message to Client
		const char* message = "Hello, Client!";
		send(clientSocket, message, static_cast<int>(strlen(message)), 0);

		// Close Sockets
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
		std::cout << "Close Socket!" << std::endl;
	}

	// Start Game Logic
	{
		Game::Get().beginPlay();

		// Main loop
		while (true)
		{
			static double tickTimer = 0.0;
			static double renderTimer = 0.0;

			static double previousTime = getCurrentTime();
			const double currentTime = getCurrentTime();
			const double elapsedTime = currentTime - previousTime;
			previousTime = currentTime;

			tickTimer += (elapsedTime * Game::Get().currentTimeScale);
			renderTimer += elapsedTime;

			while (tickTimer >= FRAME_TIME)
			{
				Game::Get().tick(static_cast<float>(tickTimer));
				tickTimer -= FRAME_TIME;
			}
		}
	}

	return 0;
}
