#pragma once

#include "SharedData.h"
#include <glm/glm.hpp>
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"
#include "GameModeManager.h"

class GameMode_Local : public GameMode
{
public:
	void beginPlay() override;
	void endPlay() override;
	void tick(float elapsedTime) override;
	void renderFrame(float elapsedTime) override;
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;

public:
	void readyRound();
	void startRound();
	void endRound();
	bool checkCollision(GameObject obj1, GameObject obj2);
	void updatePosition(float elapsedTime);
	void updatePhysics(float elapsedTime);

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
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
