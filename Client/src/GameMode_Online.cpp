#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "SharedData.h"
#include "GameMode_Online.h"
#include "GameModeManager.h"
#include "Graphics.h"

#pragma comment(lib, "ws2_32.lib")

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

	// Start Message Receiving Thread
	receiveMessageThread = std::thread(&GameMode_Online::receiveMessageFromServer, this);
}

void GameMode_Online::endPlay()
{
	// Close Sockets
	closesocket(clientSocket);
	WSACleanup();

	// Wait for Thread
	if (receiveMessageThread.joinable())
	{
		receiveMessageThread.join();
	}
}

void GameMode_Online::tick(float elapsedTime)
{
}

void GameMode_Online::renderFrame(float elapsedTime)
{
	// Object Rendering
	Graphics::Get().renderObject(player1);
	Graphics::Get().renderObject(player2);
	Graphics::Get().renderObject(ball);
	Graphics::Get().renderObject(net);
	Graphics::Get().renderObject(leftWall);
	Graphics::Get().renderObject(rightWall);
	Graphics::Get().renderObject(floor);
	Graphics::Get().renderObject(ceil);

	// Text Rendering
	std::ostringstream fpsString;
	fpsString << std::fixed << std::setprecision(1) << ( 1.0f / elapsedTime );
	Graphics::Get().renderText((fpsString.str() + " FPS").c_str(), 20.f, SCREEN_HEIGHT - 30.f, 0.25f, FPS_TEXT_COLOR);
	Graphics::Get().renderText(std::to_string(scorePlayer1), INITIAL_PLAYER1_POSITION.x, SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
	Graphics::Get().renderText(std::to_string(scorePlayer2), INITIAL_PLAYER2_POSITION.x, SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
	if ( currentRoundState == RoundStateType::Ready )
		Graphics::Get().renderText("Ready?", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - TEXT_SIZE / 2, 1.f, READY_TEXT_COLOR, true);
	if ( currentGameState == GameStateType::End )
		Graphics::Get().renderText("Game Set!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - TEXT_SIZE / 2, 1.f, GAME_SET_TEXT_COLOR, true);
}

void GameMode_Online::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	PlayerInput playerInput(key, scancode, action, mods);
	sendMessage(MessageType::PlayerInput, playerInput);
}

bool GameMode_Online::shouldClose()
{
	if (disConnected)
		return true;

	return false;
}

void GameMode_Online::onConnected()
{
	connected = true;
	disConnected = false;
}

void GameMode_Online::onDisconnected()
{
	connected = false;
	disConnected = true;
}

void GameMode_Online::onServerShutdown()
{
	connected = false;
	disConnected = true;
}

void GameMode_Online::receiveMessageFromServer()
{
	while (!shouldClose())
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(clientSocket, &readSet);

		// Get Message From Server
		select(static_cast<int>(clientSocket) + 1, &readSet, nullptr, nullptr, nullptr);
		if ( FD_ISSET(clientSocket, &readSet) )
		{
			char buffer[BUFFER_SIZE] = { 0 };
			int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
			if ( bytesReceived <= 0 )
			{
				onDisconnected();
				break;
			}
			else
			{
				messageHandler(buffer, bytesReceived);
			}
		}
	}
}

void GameMode_Online::messageHandler(char* buffer, int bytesReceived)
{
	MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer);
	if (header == nullptr)
	{
		std::cout << "Invalid Header!" << std::endl;
		return;
	}

	switch (header->type)
	{
		case MessageType::Connected:
		{
			ConnectMessage connectMessage;
			deserialize<ConnectMessage>(buffer + sizeof(MessageHeader), connectMessage);
			std::cout << "Connected to Server" << std::endl;
			onConnected();
		}
		break;
		case MessageType::Disconnected:
		{
			DisconnectMessage disconnectMessage;
			deserialize(buffer + sizeof(MessageHeader), disconnectMessage);
			std::cout << "Disconnected from Server" << std::endl;
			onDisconnected();
		}
		break;
		case MessageType::ReplicateGameState:
		{
			deserialize(buffer + sizeof(MessageHeader), replicatedGameState);
			onReplicatedGameState();
		}
		break;
		default:
		{
			std::cout << "Unhandled Message Received From Server!" << std::endl;
		}
	}
}

void GameMode_Online::onReplicatedGameState()
{
	player1.setPosition(replicatedGameState.player1Position);
	player2.setPosition(replicatedGameState.player2Position);
	ball.setPosition(replicatedGameState.ballPosition);
	scorePlayer1 = replicatedGameState.scorePlayer1;
	scorePlayer2 = replicatedGameState.scorePlayer2;
	currentGameState = replicatedGameState.currentGameState;
	currentRoundState = replicatedGameState.currentRoundState;
}
