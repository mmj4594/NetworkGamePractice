#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <WS2tcpip.h>
#include "GameMode_Online.h"
#include "GameModeManager.h"
#include "Graphics.h"

constexpr int BUFFER_SIZE = 1024;

void GameMode_Online::beginPlay()
{
	// Connnect to Server
	WSADATA wsaData;
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

	// Loop
	while (true)
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(clientSocket, &readSet);

		// Get Data From Server
		select(static_cast<int>(clientSocket) + 1, &readSet, nullptr, nullptr, nullptr);
		if ( FD_ISSET(clientSocket, &readSet) )
		{
			char buffer[BUFFER_SIZE] = { 0 };
			int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
			if ( bytesReceived <= 0 )
			{
				std::cout << "Disconnect Server!" << std::endl;
				break;
			}
			else
			{
				std::cout << "Message from Server: " << buffer << std::endl;
			}
		}
	}
}

void GameMode_Online::endPlay()
{
	// Close Sockets
	closesocket(clientSocket);
	WSACleanup();
}

void GameMode_Online::tick(float elapsedTime)
{
	// Send Data to Server
	const char* message = "Hello, Server!";
	send(clientSocket, message, static_cast<int>(strlen(message)), 0);
	std::cout << "Send Message to Server: " << message << std::endl;
}

void GameMode_Online::renderFrame(float elapsedTime)
{
	// Text Rendering
	std::ostringstream fpsString;
	fpsString << std::fixed << std::setprecision(1) << ( 1.0f / elapsedTime );
	Graphics::Get().renderText(( fpsString.str() + " FPS" ).c_str(), 20.f, 570.f, 0.25f, FPS_TEXT_COLOR);
}

void GameMode_Online::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
}
