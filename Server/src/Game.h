#pragma once

#include <glm/glm.hpp>
#include "SharedData.h"
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"

struct GLFWwindow;

class Game
{
public:
	static Game& Get();

public:
	void beginPlay();
	void endPlay();
	void tick(float elapsedTime);
	void onKey(int playerID, int key, int scancode, int action, int mods);

public:
	void readyRound();
	void startRound();
	void endRound();
	bool checkCollision(GameObject obj1, GameObject obj2);
	void updatePosition(float elapsedTime);
	void updatePhysics(float elapsedTime);

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
	GameStateType currentGameState = GameStateType::None;
	RoundStateType currentRoundStateType = RoundStateType::None;
	float currentTimeScale = BASIC_TIME_SCALE;
	int lastRoundWinnerPlayerID = -1;

private:
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
