#pragma once

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

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
	bool checkCollision(float obj1X, float obj1Y, float obj1Width, float obj1Height, float obj2X, float obj2Y, float obj2Width, float obj2Height);
	void updatePhysics();
	void OnPressedKey(int key, int scancode, int action, int mods);

public:
	float player1X = -0.8f, player1Y = -0.8f, player1SpeedY = 0.0f;
	float player2X = 0.8f, player2Y = -0.8f, player2SpeedY = 0.0f;
	float ballX = 0.0f, ballY = 0.0f, ballSpeedX = 0.01f, ballSpeedY = 0.02f;
	int scorePlayer1 = 0, scorePlayer2 = 0;
	float netX = 0.0f, netY = -0.8f, netWidth = 0.05f, netHeight = 1.0f;

	bool player1Jumping = false, player2Jumping = false;
};
