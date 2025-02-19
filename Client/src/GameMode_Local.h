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
	RoundStateType currentRoundState = RoundStateType::None;
	int lastRoundWinnerPlayerID = -1;

private:
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
