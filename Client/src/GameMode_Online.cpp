#include "SharedData.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "GameMode_Online.h"
#include "GameModeManager.h"
#include "Graphics.h"

#pragma comment(lib, "ws2_32.lib")

LogCategory LogGameModeOnline("GameModeOnline");

void GameMode_Online::beginPlay()
{
	LOG(LogGameModeOnline, LogVerbosity::Log, "Online Mode is Started");

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
		LOG(LogGameModeOnline, LogVerbosity::Error, "beginPlay: Failed to Connect to Server!");
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

	LOG(LogGameModeOnline, LogVerbosity::Log, "Online Mode is Terminated");
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
	sendMessage(clientSocket, MessageType::PlayerInput, playerInput);
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
			char buffer[MAX_BUFFER_SIZE] = { 0 };
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
		LOG(LogGameModeOnline, LogVerbosity::Error, "messageHandler: Invalid Header!");
		return;
	}

	switch (header->type)
	{
		case MessageType::Connected:
		{
			ConnectMessage connectMessage;
			deserialize(buffer + sizeof(MessageHeader), connectMessage);
			LOG(LogGameModeOnline, LogVerbosity::Log, "Connected to Server!");
			onConnected();
		}
		break;
		case MessageType::Disconnected:
		{
			DisconnectMessage disconnectMessage;
			deserialize(buffer + sizeof(MessageHeader), disconnectMessage);
			LOG(LogGameModeOnline, LogVerbosity::Log, "Disconnected from Server");
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
			LOG(LogGameModeOnline, LogVerbosity::Error, "messageHandler: Unhandled Message Received From Server!");
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
	lastRoundWinnerPlayerID = replicatedGameState.lastRoundWinnerPlayerID;
	if (currentRoundState != replicatedGameState.currentRoundState)
	{
		switch (replicatedGameState.currentRoundState)
		{
		case RoundStateType::Ready:
			LOG(LogGameModeOnline, LogVerbosity::Log, "Ready Round %d", scorePlayer1 + scorePlayer2 + 1);
			break;
		case RoundStateType::Playing:
			LOG(LogGameModeOnline, LogVerbosity::Log, "Start Round %d", scorePlayer1 + scorePlayer2 + 1);
			break;
		case RoundStateType::End:
			LOG(LogGameModeOnline, LogVerbosity::Log, "End Round %d. Current Round Score: [%d:%d]. Round Winner Player: %d",
				scorePlayer1 + scorePlayer2, scorePlayer1, scorePlayer2, lastRoundWinnerPlayerID);
			break;
		default:
			LOG(LogGameModeOnline, LogVerbosity::Error, "onReplicatedGameState: Unhandled Round State Type");
		}
		currentRoundState = replicatedGameState.currentRoundState;
	}
	if (currentGameState != replicatedGameState.currentGameState)
	{
		switch (replicatedGameState.currentGameState)
		{
		case GameStateType::Ready:
			LOG(LogGameModeOnline, LogVerbosity::Log, "Game is Ready");
			break;
		case GameStateType::Playing:
			LOG(LogGameModeOnline, LogVerbosity::Log, "Game is Started");
			break;
		case GameStateType::End:
			LOG(LogGameModeOnline, LogVerbosity::Log, "Game is Finished! Winner Player: %d", scorePlayer1 >= MAX_SCORE ? player1.getPlayerID() : player2.getPlayerID());
			break;
		default:
			LOG(LogGameModeOnline, LogVerbosity::Error, "onReplicatedGameState: Unhandled Game State Type");
		}
		currentGameState = replicatedGameState.currentGameState;
	}
}
