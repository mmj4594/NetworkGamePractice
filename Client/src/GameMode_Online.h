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
	Player player1 = Player(1, INITIAL_PLAYER1_POSITION, 75.f, 75.f);
	Player player2 = Player(2, INITIAL_PLAYER2_POSITION, 75.f, 75.f);
	Ball ball = Ball(INITIAL_BALL_POSITION, 60.f, 60.f);
	Net net = Net(INITIAL_NET_POSITION, NET_WIDTH, NET_HEIGHT);
	Block leftWall = Block(INITIAL_LEFT_WALL_POSITION, BLOCK_THICKNESS, SCREEN_HEIGHT);
	Block rightWall = Block(INITIAL_RIGHT_WALL_POSITION, BLOCK_THICKNESS, SCREEN_HEIGHT);
	Block floor = Block(INITIAL_FLOOR_POSITION, SCREEN_WIDTH, BLOCK_THICKNESS);
	Block ceil = Block(INITIAL_CEIL_POSITION, SCREEN_WIDTH, BLOCK_THICKNESS);

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
