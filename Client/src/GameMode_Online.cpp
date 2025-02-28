#include "SharedData.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
	serverAddr.sin_port = htons(Config::Get().SERVERPORT);
	const char* serverIPAddr = Config::Get().SERVERHOST.c_str();
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
	// FPS
	std::ostringstream fpsString;
	fpsString << std::fixed << std::setprecision(1) << ( 1.0f / elapsedTime );
	Graphics::Get().renderText(( fpsString.str() + " FPS" ).c_str(), 20.f, Config::Get().SCREEN_HEIGHT - 30.f, 0.25f, FPS_TEXT_COLOR);

	switch(currentGameState)
	{
	case GameStateType::None:
		if (!connected && !disConnected)
		{
			Graphics::Get().renderText("Failed to Connect Server.", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, EXCEPTION_TEXT_COLOR, true);
			printExitText();
		}
		else if(connected)
		{
			Graphics::Get().renderText("Waiting for Another Player...", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, READY_TEXT_COLOR, true);
		}
		else if (disConnected)
		{
			printDisconnectedText();
			printExitText();
		}
		break;

	case GameStateType::Ready:
		if (disConnected)
		{
			printDisconnectedText();
			printExitText();
		}
		else
		{
			Graphics::Get().renderText("Game Ready!", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f + 25, 1.f, READY_TEXT_COLOR, true);
			glm::vec3 playerColor = myPlayerID == player1.getPlayerID() ? P1_COLOR : P2_COLOR;
			Graphics::Get().renderText("You are Player " + std::to_string(myPlayerID), Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - 25, 0.7f, playerColor, true);
		}
		break;

	case GameStateType::Playing:
		if (disConnected)
		{
			printDisconnectedText();
			printExitText();
		}
		else
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
			Graphics::Get().renderText(std::to_string(scorePlayer1), Config::Get().INITIAL_PLAYER1_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
			Graphics::Get().renderText(std::to_string(scorePlayer2), Config::Get().INITIAL_PLAYER2_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
			if ( currentRoundState == RoundStateType::Ready )
				Graphics::Get().renderText("Ready?", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, READY_TEXT_COLOR, true);
		}
		break;

	case GameStateType::End:
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
		Graphics::Get().renderText(std::to_string(scorePlayer1), Config::Get().INITIAL_PLAYER1_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
		Graphics::Get().renderText(std::to_string(scorePlayer2), Config::Get().INITIAL_PLAYER2_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
		Graphics::Get().renderText("Game Set!", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, GAME_SET_TEXT_COLOR, true);
		printExitText();
		break;
	}
}

void GameMode_Online::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(connected && currentGameState == GameStateType::Playing)
	{
		PlayerInput playerInput(key, scancode, action, mods);
		sendMessage(clientSocket, MessageType::PlayerInput, playerInput);
	}

	if (!connected || disConnected)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			Graphics::Get().reserveClose();
		}
	}
}

void GameMode_Online::onConnected(int connectedPlayerID)
{
	connected = true;
	disConnected = false;
	myPlayerID = connectedPlayerID;
}

void GameMode_Online::onDisconnected()
{
	connected = false;
	disConnected = true;
}

void GameMode_Online::receiveMessageFromServer()
{
	while (!disConnected)
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
			onConnected(connectMessage.connectedPlayerID);
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
			ReplicatedGameState replicatedGameState;
			deserialize(buffer + sizeof(MessageHeader), replicatedGameState);
			onReplicatedGameState(replicatedGameState);
		}
		break;
		default:
		{
			LOG(LogGameModeOnline, LogVerbosity::Error, "messageHandler: Unhandled Message Received From Server!");
		}
	}
}

void GameMode_Online::onReplicatedGameState(const ReplicatedGameState& replicatedGameState)
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
			LOG(LogGameModeOnline, LogVerbosity::Log, "Game is Finished! Winner Player: %d", scorePlayer1 >= Config::Get().MAX_SCORE ? player1.getPlayerID() : player2.getPlayerID());
			break;
		default:
			LOG(LogGameModeOnline, LogVerbosity::Error, "onReplicatedGameState: Unhandled Game State Type");
		}
		currentGameState = replicatedGameState.currentGameState;
	}
}

void GameMode_Online::printDisconnectedText()
{
	Graphics::Get().renderText("Disconnected from Server.", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, EXCEPTION_TEXT_COLOR, true);
}

void GameMode_Online::printExitText()
{
	Graphics::Get().renderText("Press ESC or click the 'X' button to exit.", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - 75, 0.7f, EXCEPTION_TEXT_COLOR, true);
}
