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
	Player player1 = Player(1, glm::vec2(70.f, BLOCK_THICKNESS / 2 + 37.5f), 75.f, 75.f);
	Player player2 = Player(2, glm::vec2(SCREEN_WIDTH - 70.f, BLOCK_THICKNESS / 2 + 37.5f), 75.f, 75.f);
	Ball ball = Ball(glm::vec2(100.f, SCREEN_HEIGHT - BLOCK_THICKNESS / 2 - 30.f), 60.f, 60.f);
	Net net = Net(glm::vec2(SCREEN_WIDTH / 2, NET_HEIGHT / 2), NET_WIDTH, NET_HEIGHT);
	Block leftWall = Block(glm::vec2(0.f, SCREEN_HEIGHT / 2), BLOCK_THICKNESS, SCREEN_HEIGHT);
	Block rightWall = Block(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT / 2), BLOCK_THICKNESS, SCREEN_HEIGHT);
	Block floor = Block(glm::vec2(SCREEN_WIDTH / 2, 0.f), SCREEN_WIDTH, BLOCK_THICKNESS);
	Block ceil = Block(glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT), SCREEN_WIDTH, BLOCK_THICKNESS);

	int scorePlayer1 = 0, scorePlayer2 = 0;
	GameStateType currentGameState = GameStateType::None;
	RoundStateType currentRoundStateType = RoundStateType::None;
	float currentTimeScale = BASIC_TIME_SCALE;
	int lastRoundWinnerPlayerID = -1;

private:
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
