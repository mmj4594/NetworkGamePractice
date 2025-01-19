#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;

constexpr float GRAVITY = -1.0f;
constexpr glm::vec2 BALL_MAX_SPEED = glm::vec2(3.f, 3.f);
constexpr glm::vec2 BALL_MIN_SPEED = glm::vec2(-1.f, -1.f);
constexpr glm::vec2 PLAYER_MAX_SPEED = glm::vec2(1.f, 1.f);
constexpr glm::vec2 PLAYER_MIN_SPEED = glm::vec2(-1.f, -1.f);
constexpr glm::vec2 PLAYER_SLIDING_MAX_SPEED = glm::vec2(2.f, 2.f);
constexpr glm::vec2 PLAYER_SLIDING_MIN_SPEED = glm::vec2(-2.f, -2.f);
constexpr float PLYAER_JUMP_SPEED = 1.0f;
constexpr float PLAYER_SLIDING_SPEED = 2.f;
constexpr float PLAYER_SLIDING_DURATION = 0.7f;
constexpr float PLAYER_FRICTION = 5.f;
constexpr float PLAYER_MOVE_ACC = 5.f;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float TIME_SCALE = 1.0f;

struct GLFWwindow;

class Game
{
public:
	static Game& Get();

public:
	void beginPlay();
	void resetRound();
	void tick(float elapsedTime);
	bool checkCollision(GameObject obj1, GameObject obj2);
	void updatePhysics(float elapsedTime);
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	Player player1 = Player(1, glm::vec2(-0.8f, -0.8f), 0.15f, 0.15f);
	Player player2 = Player(2, glm::vec2(0.8f, -0.8f), 0.15f, 0.15f);
	Ball ball = Ball(glm::vec2(0.f, 0.f), 0.13f, 0.13f);
	Net net = Net(glm::vec2(0.f, -0.8f), 0.05f, 1.0f);
	Block leftWall = Block(glm::vec2(-1.0f, 0.f), 0.05f, 2.f);
	Block rightWall = Block(glm::vec2(1.f, 0.f), 0.05f, 2.f);
	Block floor = Block(glm::vec2(0.f, -1.f), 2.f, 0.05f);

	int scorePlayer1 = 0, scorePlayer2 = 0;
};
