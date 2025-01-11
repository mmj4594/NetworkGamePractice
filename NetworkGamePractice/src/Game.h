#pragma once

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

extern float player1X, player1Y, player1SpeedY;
extern float player2X, player2Y, player2SpeedY;
extern float ballX, ballY, ballSpeedX, ballSpeedY;
extern int scorePlayer1, scorePlayer2;
extern float netX, netY, netWidth, netHeight;

constexpr float GRAVITY = -0.0005f;
constexpr float JUMP_SPEED = 0.02f;
extern bool player1Jumping, player2Jumping;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
extern double previousTime;
extern double lag;
const float timeScale = 1.0f;

void resetRound();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool checkCollision(float obj1X, float obj1Y, float obj1Width, float obj1Height, float obj2X, float obj2Y, float obj2Width, float obj2Height);
void updatePhysics();