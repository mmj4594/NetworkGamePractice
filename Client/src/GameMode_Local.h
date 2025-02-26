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
	Player player1 = Player(1, INITIAL_PLAYER1_POSITION, PLAYER_SIZE.x, PLAYER_SIZE.y);
	Player player2 = Player(2, INITIAL_PLAYER2_POSITION, PLAYER_SIZE.x, PLAYER_SIZE.y);
	Ball ball = Ball(INITIAL_BALL_POSITION, BALL_SIZE.x, BALL_SIZE.y);
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
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
