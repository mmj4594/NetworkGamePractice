#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Graphics.h"
#include "GameModeManager.h"

#pragma comment(lib, "ws2_32.lib")

float getCurrentTime()
{
	using namespace std::chrono;
	return duration<float>(steady_clock::now().time_since_epoch()).count();
}

int main()
{
	//// Connnect to Server
	//WSADATA wsaData;
	//SOCKET clientSocket;
	//struct sockaddr_in serverAddr;
	//{
	//	// Windows Socket API Startup
	//	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//	// Cretae Client Socket(IPv4, TCP) and Set Target Server Address
	//	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	//	serverAddr.sin_family = AF_INET;
	//	serverAddr.sin_port = htons(9000);
	//	const char* serverIPAddr = "127.0.0.1";
	//	inet_pton(AF_INET, serverIPAddr, &serverAddr.sin_addr);

	//	// Try to Connect to Server
	//	if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	//	{
	//		std::cerr << "Failed to Connect to Server!" << std::endl;
	//		return 1;
	//	}
	//	std::cout << "Connected to Server!" << std::endl;

	//	// Send Message to Server
	//	const char* message = "Hello, Server!";
	//	send(clientSocket, message, static_cast<int>(strlen(message)), 0);

	//	// Receive Message from Server
	//	char buffer[1024] = { 0 };
	//	recv(clientSocket, buffer, sizeof(buffer), 0);
	//	std::cout << "Message from Server: " << buffer << std::endl;
	//}

	// Start Game
	{
		if (Graphics::Get().initializeGraphics() == false)
		{
			std::cerr << "Failed to initialize Graphics!" << std::endl;
			return -1;
		}

		GameModeManager::Get().beginPlay();

		// Main loop
		while (!Graphics::Get().shouldClose())
		{
			static double tickTimer = 0.0;
			static double renderTimer = 0.0;

			static double previousTime = getCurrentTime();
			const double currentTime = getCurrentTime();
			const double elapsedTime = currentTime - previousTime;
			previousTime = currentTime;

			tickTimer += (elapsedTime * GameModeManager::Get().currentTimeScale);
			renderTimer += elapsedTime;

			while (tickTimer >= FRAME_TIME)
			{
				GameModeManager::Get().tick(static_cast<float>(tickTimer));
				tickTimer -= FRAME_TIME;
			}

			while (renderTimer >= FRAME_TIME)
			{
				Graphics::Get().renderFrame(static_cast<float>(renderTimer));
				renderTimer -= FRAME_TIME;
			}
		}

		GameModeManager::Get().endPlay();
	}

	glfwTerminate();

	//// Close Sockets
	//closesocket(clientSocket);
	//WSACleanup();

	return 0;
}