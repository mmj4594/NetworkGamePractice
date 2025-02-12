#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"
#include "GameState.h"

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

constexpr float GRAVITY = -1000.f;
constexpr glm::vec2 BALL_SPEED_RANGE = glm::vec2(-2000.f, 2000.f);
constexpr float BALL_SPIKE_SPEED = 500.f;
constexpr glm::vec2 PLAYER_SPEED_RANGE = glm::vec2(-400.f, 400.f);
constexpr glm::vec2 PLAYER_SLIDING_SPEED_RANGE = glm::vec2(-1000.f, 1000.f);
constexpr float PLYAER_JUMP_SPEED = 750.0f;
constexpr float PLAYER_SLIDING_SPEED = 870.f;
constexpr float PLAYER_SLIDING_DURATION = 0.7f;
constexpr float PLAYER_FRICTION = 2000.f;
constexpr float PLAYER_MOVE_ACC = 1000.f;
constexpr float NORMAL_COLLISION_IMPACT_FACTOR = 9.3f;

constexpr float NET_WIDTH = 6.f;
constexpr float NET_HEIGHT = 200.f;
constexpr float BLOCK_THICKNESS = 30.f;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float TIME_SCALE = 1.0f;

constexpr int MAX_SCORE = 15;
constexpr float ROUND_END_TIME_SCALE = 0.15f;
constexpr float ROUND_WAIT_TIME = 2.0f;
constexpr float ROUND_END_TIME = 2.0f * ROUND_END_TIME_SCALE;
enum class GamePlayState
{
	None,
	Playing,
	End,
};
enum class RoundPlayState
{
	None,
	Ready,
	Playing,
	End,
};

constexpr int TEXT_SIZE = 48;
constexpr glm::vec3 FPS_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 SCORE_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 READY_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 GAME_SET_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BACKGROUND_COLOR = glm::vec3(0.3f, 0.3f, 0.3f);
constexpr glm::vec3 P1_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 P2_COLOR = glm::vec3(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 BALL_COLOR = glm::vec3(1.0f, 1.0f, 0.0f);
constexpr glm::vec3 NET_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLOCK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

class GameState_Local : public GameState
{
public:
	void beginPlay() override;
	void tick(float elapsedTime) override;
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
	GamePlayState currentGameState = GamePlayState::None;
	RoundPlayState currentRoundState = RoundPlayState::None;
	float currentTimeScale = TIME_SCALE;
	int lastRoundWinnerPlayerID = -1;

private:
	float roundWaitTimer = 0.f;
	float roundEndTimer = 0.f;
};
