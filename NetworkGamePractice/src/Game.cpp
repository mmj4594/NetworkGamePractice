#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

Game& Game::Get()
{
	static Game instance;
	return instance;
}

void Game::resetRound()
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

void Game::tick(double elapsedTime)
{
	updatePhysics();
}

bool Game::checkCollision(float obj1X, float obj1Y, float obj1Width, float obj1Height, float obj2X, float obj2Y, float obj2Width, float obj2Height)
{
	return obj1X < obj2X + obj2Width
		&& obj1X + obj1Width > obj2X
		&& obj1Y < obj2Y + obj2Height
		&& obj1Y + obj1Height > obj2Y;
}

void Game::updatePhysics()
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

void Game::OnPressedKey(int key, int scancode, int action, int mods)
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
