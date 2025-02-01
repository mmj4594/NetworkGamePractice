#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Ball.h"
#include "Net.h"
#include "Block.h"

constexpr int SCREEN_WIDTH = 800;
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
constexpr float COLLISION_IMPACT_FACTOR = 8.f;


constexpr int MAX_ROUNDS = 15;

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
	void updatePosition(float elapsedTime);
	void updatePhysics(float elapsedTime);
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	Player player1 = Player(1, glm::vec2(70.f, 37.5f), 75.f, 75.f);
	Player player2 = Player(2, glm::vec2(730.f, 37.5f), 75.f, 75.f);
	Ball ball = Ball(glm::vec2(100.f, 600.f), 60.f, 60.f);
	Net net = Net(glm::vec2(400.f, 100.f), 6.f, 200.f);
	Block leftWall = Block(glm::vec2(0.f, 300.f), 30.f, 600.f);
	Block rightWall = Block(glm::vec2(800.f, 300.f), 30.f, 600.f);
	Block floor = Block(glm::vec2(400.f, 0.f), 800.f, 30.f);
	Block ceil = Block(glm::vec2(400.f, 600.f), 800.f, 30.f);

	int scorePlayer1 = 0, scorePlayer2 = 0;
};
