#pragma once

#include <glm/glm.hpp>
#include "Player.h"
#include "Ball.h"
#include "Net.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 800;

constexpr float GRAVITY = -0.0005f;
constexpr float JUMP_SPEED = 0.02f;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float TIME_SCALE = 1.0f;

class Game
{
public:
	static Game& Get();

public:
	void resetRound();
	void tick(double elapsedTime);
	bool checkCollision(GameObject obj1, GameObject obj2);
	void updatePhysics();
	void OnPressedKey(int key, int scancode, int action, int mods);

public:
	Player player1 = Player(1, glm::vec2(-0.8f, -0.8f), 0.15f, 0.15f);
	Player player2 = Player(2, glm::vec2(0.8f, -0.8f), 0.15f, 0.15f);
	Ball ball = Ball(glm::vec2(0.f, 0.f), 0.13f, 0.13f);
	Net net = Net(glm::vec2(0.f, -0.8f), 0.05f, 1.0f);

	int scorePlayer1 = 0, scorePlayer2 = 0;
};
