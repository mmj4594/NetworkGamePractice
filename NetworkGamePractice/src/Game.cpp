#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "game.h"

float player1X = -0.8f, player1Y = -0.8f, player1SpeedY = 0.0f;
float player2X = 0.8f, player2Y = -0.8f, player2SpeedY = 0.0f;
float ballX = 0.0f, ballY = 0.0f, ballSpeedX = 0.01f, ballSpeedY = 0.02f;
int scorePlayer1 = 0, scorePlayer2 = 0;
float netX = 0.0f, netY = -0.8f, netWidth = 0.05f, netHeight = 1.0f;

bool player1Jumping = false, player2Jumping = false;

double previousTime = glfwGetTime();
double lag = 0;

void resetRound()
{
	ballX = 0.0f;
	ballY = 0.0f;
	ballSpeedX = 0.01f * (rand() % 2 == 0 ? 1 : -1);
	ballSpeedY = 0.02f;
	player1X = -0.8f;
	player1Y = -0.8f;
	player2X = 0.8f;
	player2Y = -0.8f;
	player1Jumping = player2Jumping = false;
	printf("Current Score: [%d : %d]\n", scorePlayer1, scorePlayer2);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS && !player1Jumping)
	{
		player1SpeedY = JUMP_SPEED;
		player1Jumping = true;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2Jumping)
	{
		player2SpeedY = JUMP_SPEED;
		player2Jumping = true;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) player1X -= 0.02f;
	if (key == GLFW_KEY_D && action == GLFW_PRESS) player1X += 0.02f;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) player2X -= 0.02f;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) player2X += 0.02f;
}

bool checkCollision(float obj1X, float obj1Y, float obj1Width, float obj1Height, float obj2X, float obj2Y, float obj2Width, float obj2Height)
{
	return obj1X < obj2X + obj2Width
		&& obj1X + obj1Width > obj2X
		&& obj1Y < obj2Y + obj2Height
		&& obj1Y + obj1Height > obj2Y;
}

void updatePhysics()
{
	// player1 Jump
	if (player1Jumping)
	{
		player1Y += player1SpeedY;
		player1SpeedY += GRAVITY;
		if (player1Y <= -0.8f)
		{
			player1Y = -0.8f;
			player1Jumping = false;
		}
	}
	// player2 Jump
	if (player2Jumping)
	{
		player2Y += player2SpeedY;
		player2SpeedY += GRAVITY;
		if (player2Y <= -0.8f)
		{
			player2Y = -0.8f;
			player2Jumping = false;
		}
	}

	// ball physics
	ballX += ballSpeedX;
	ballY += ballSpeedY;
	ballSpeedY += GRAVITY;

	// ball - floor
	if (ballY <= -1.0f)
	{
		if (ballX < 0.0f)
		{
			scorePlayer2++;
		}
		else
		{
			scorePlayer1++;
		}
		resetRound();
	}

	// ball - net
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, netX - netWidth / 2, netY, netWidth, netHeight))
	{
		ballSpeedX = -ballSpeedX;
	}

	// ball - player
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, player1X, player1Y, 0.2f, 0.2f))
	{
		ballSpeedY = 0.015f;
		ballSpeedX = 0.01f;
	}
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, player2X, player2Y, 0.2f, 0.2f))
	{
		ballSpeedY = 0.015f;
		ballSpeedX = -0.01f;
	}
}