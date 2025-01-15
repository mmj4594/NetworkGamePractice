#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Game.h"

Game& Game::Get()
{
	static Game instance;
	return instance;
}

void Game::beginPlay()
{
	player1.beginPlay();
	player2.beginPlay();
	ball.beginPlay();
	net.beginPlay();
	leftWall.beginPlay();
	rightWall.beginPlay();
	floor.beginPlay();
}

void Game::resetRound()
{
	player1.reset();
	player2.reset();
	ball.reset();
	ball.setSpeed(glm::vec2(0.01f * (rand() % 2 == 0 ? 1 : -1), 0.02f));
	printf("Current Score: [%d : %d]\n", scorePlayer1, scorePlayer2);
}

void Game::tick(double elapsedTime)
{
	updatePhysics();
}

bool Game::checkCollision(GameObject obj1, GameObject obj2)
{
	return obj1.getPosition().x + obj1.getWidth()/2 > obj2.getPosition().x - obj2.getWidth()/2
		&& obj1.getPosition().x - obj1.getWidth()/2 < obj2.getPosition().x + obj2.getWidth()/2
		&& obj1.getPosition().y + obj1.getHeight()/2 > obj2.getPosition().y - obj2.getHeight()/2
		&& obj1.getPosition().y - obj1.getHeight()/2 < obj2.getPosition().y + obj2.getHeight()/2;
}

void Game::updatePhysics()
{
	// player1
	{
		glm::vec2 player1Position = player1.getPosition();
		glm::vec2 player1Speed = player1.getSpeed();
		player1Position.y += player1Speed.y;
		player1Speed.y += GRAVITY;
		if (player1Position.y - player1.getHeight() / 2 <= floor.getPosition().y + floor.getHeight() / 2)
		{
			player1Position.y = floor.getPosition().y + floor.getHeight() / 2 + player1.getHeight() / 2;
			player1.setJumping(false);
		}
		player1.setPosition(player1Position);
		player1.setSpeed(player1Speed);
	}
	// player2
	{
		glm::vec2 player2Position = player2.getPosition();
		glm::vec2 player2Speed = player2.getSpeed();
		player2Position.y += player2Speed.y;
		player2Speed.y += GRAVITY;
		if (player2Position.y - player2.getHeight() / 2 <= floor.getPosition().y + floor.getHeight() / 2 )
		{
			player2Position.y = floor.getPosition().y + floor.getHeight() / 2 + player2.getHeight() / 2;
			player2.setJumping(false);
		}
		player2.setPosition(player2Position);
		player2.setSpeed(player2Speed);
	}

	// ball physics
	glm::vec2 ballPosition = ball.getPosition();
	glm::vec2 ballSpeed = ball.getSpeed();
	ballPosition += ballSpeed;
	ballSpeed.y += GRAVITY;
	ball.setPosition(ballPosition);
	ball.setSpeed(ballSpeed);

	// ball - floor
	if (checkCollision(ball, floor))
	{
		if (ballPosition.x < 0.0f)
		{
			scorePlayer2++;
		}
		else
		{
			scorePlayer1++;
		}
		resetRound();
		return;
	}

	// ball - net
	if (checkCollision(ball, net))
	{
		ballSpeed.x = -ballSpeed.x;
		ball.setSpeed(ballSpeed);
	}

	// ball - player
	if (checkCollision(ball, player1))
	{
		ballSpeed.x = 0.01f;
		ballSpeed.y = 0.015f;
		ball.setSpeed(ballSpeed);
	}
	if (checkCollision(ball, player2))
	{
		ballSpeed.x = -0.01f;
		ballSpeed.y = 0.015f;
		ball.setSpeed(ballSpeed);
	}

	// ball - wall
	if (checkCollision(ball, leftWall) || checkCollision(ball, rightWall))
	{
		ballSpeed.x = -ballSpeed.x;
		ball.setSpeed(ballSpeed);
	}
}

void Game::OnPressedKey(int key, int scancode, int action, int mods)
{
	// jump
	if (key == GLFW_KEY_R && action == GLFW_PRESS && !player1.getJumping()) player1.jump();
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2.getJumping()) player2.jump();

	// move
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) player1.moveLeft();
	if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT)) player1.moveRight();
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) player2.moveLeft();
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) player2.moveRight();
}
