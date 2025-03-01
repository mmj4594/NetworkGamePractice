#pragma once

#include "SharedData.h"
#include <thread>
#include <WinSock2.h>
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"
#include "GameMode.h"

class GameMode_Online : public GameMode
{
public:
	void beginPlay() override;
	void endPlay() override;
	void tick(float elapsedTime) override;
	void renderFrame(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;

public:
	void onConnected(int connectedPlayerID);
	void onDisconnected();
	void receiveMessageFromServer();
	void messageHandler(char* buffer, int bytesReceived);
	void onReplicatedGameState(const ReplicatedGameState& replicatedGameState);

private:
	void printDisconnectedText();
	void printExitText();

public:
	Player player1 = Player(1, Config::Get().INITIAL_PLAYER1_POSITION, Config::Get().PLAYER_SIZE.x, Config::Get().PLAYER_SIZE.y);
	Player player2 = Player(2, Config::Get().INITIAL_PLAYER2_POSITION, Config::Get().PLAYER_SIZE.x, Config::Get().PLAYER_SIZE.y);
	Ball ball = Ball(Config::Get().INITIAL_BALL_POSITION, Config::Get().BALL_SIZE.x, Config::Get().BALL_SIZE.y);
	Net net = Net(Config::Get().INITIAL_NET_POSITION, (float)Config::Get().NET_WIDTH, (float)Config::Get().NET_HEIGHT);
	Block leftWall = Block(Config::Get().INITIAL_LEFT_WALL_POSITION, (float)Config::Get().BLOCK_THICKNESS, (float)Config::Get().SCREEN_HEIGHT);
	Block rightWall = Block(Config::Get().INITIAL_RIGHT_WALL_POSITION, (float)Config::Get().BLOCK_THICKNESS, (float)Config::Get().SCREEN_HEIGHT);
	Block floor = Block(Config::Get().INITIAL_FLOOR_POSITION, (float)Config::Get().SCREEN_WIDTH, (float)Config::Get().BLOCK_THICKNESS);
	Block ceil = Block(Config::Get().INITIAL_CEIL_POSITION, (float)Config::Get().SCREEN_WIDTH, (float)Config::Get().BLOCK_THICKNESS);

	int scorePlayer1 = 0, scorePlayer2 = 0;
	int lastRoundWinnerPlayerID = -1;
	GameStateType currentGameState = GameStateType::None;
	RoundStateType currentRoundState = RoundStateType::None;

private:
	SOCKET clientSocket;
	std::thread receiveMessageThread;
	bool connected = false;
	bool disConnected = false;
	int myPlayerID = -1;
};
