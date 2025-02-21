#pragma once

#include <glm/glm.hpp>
#include "SharedData.h"
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"
#include "GameModeManager.h"

constexpr glm::vec3 SCORE_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 READY_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 GAME_SET_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 P1_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 P2_COLOR = glm::vec3(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 BALL_COLOR = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 NET_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLOCK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

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
	RoundStateType currentRoundState = RoundStateType::None;
	int lastRoundWinnerPlayerID = -1;

private:
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
