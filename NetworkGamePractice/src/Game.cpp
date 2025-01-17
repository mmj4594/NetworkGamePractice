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
	ball.addImpulse(glm::vec2(0.3f * (rand() % 2 == 0 ? 1 : -1), 1.0f));
	printf("Current Score: [%d : %d]\n", scorePlayer1, scorePlayer2);
}

void Game::tick(float elapsedTime)
{
	updatePhysics(elapsedTime);
}

bool Game::checkCollision(GameObject obj1, GameObject obj2)
{
	return obj1.getPosition().x + obj1.getWidth()/2 > obj2.getPosition().x - obj2.getWidth()/2
		&& obj1.getPosition().x - obj1.getWidth()/2 < obj2.getPosition().x + obj2.getWidth()/2
		&& obj1.getPosition().y + obj1.getHeight()/2 > obj2.getPosition().y - obj2.getHeight()/2
		&& obj1.getPosition().y - obj1.getHeight()/2 < obj2.getPosition().y + obj2.getHeight()/2;
}

void Game::updatePhysics(float elapsedTime)
{
	player1.updatePosition(elapsedTime);
	player1.updateSpeed(elapsedTime);

	player2.updatePosition(elapsedTime);
	player2.updateSpeed(elapsedTime);

	ball.updatePosition(elapsedTime);
	ball.updateSpeed(elapsedTime);

	// ball - floor
	if (checkCollision(ball, floor))
	{
		if (ball.getPosition().x < 0.0f)
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
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		return;
	}

	// ball - player
	if (checkCollision(ball, player1))
	{
		ball.setSpeed(glm::vec2(1.f, 1.5f));
		return;
	}
	if (checkCollision(ball, player2))
	{
		ball.setSpeed(glm::vec2(-1.f, 1.5f));
		return;
	}

	// ball - wall
	if (checkCollision(ball, leftWall) || checkCollision(ball, rightWall))
	{
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		return;
	}
}

void Game::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// jump
	if (key == GLFW_KEY_R && action == GLFW_PRESS && !player1.getJumping()) player1.jump();
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2.getJumping()) player2.jump();

	// player1 move
	static bool DPressed = false;
	static bool GPressed = false;
	if (key == GLFW_KEY_D && action == GLFW_PRESS) DPressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) DPressed = false;
	if (key == GLFW_KEY_G && action == GLFW_PRESS) GPressed = true;
	if (key == GLFW_KEY_G && action == GLFW_RELEASE) GPressed = false;
	if (DPressed && GPressed) player1.setAcc(glm::vec2(0, player1.getAcc().y));
	else if (DPressed) player1.setAcc(glm::vec2(-PLAYER_MOVE_ACC, player1.getAcc().y));
	else if (GPressed) player1.setAcc(glm::vec2(PLAYER_MOVE_ACC, player1.getAcc().y));
	else player1.setAcc(glm::vec2(0, player1.getAcc().y));

	// player2 move
	static bool leftPressed = false;
	static bool rightPressed = false;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) leftPressed = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) leftPressed = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) rightPressed = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) rightPressed = false;
	if (leftPressed && rightPressed) player2.setAcc(glm::vec2(0, player2.getAcc().y));
	else if (leftPressed) player2.setAcc(glm::vec2(-PLAYER_MOVE_ACC, player2.getAcc().y));
	else if (rightPressed) player2.setAcc(glm::vec2(PLAYER_MOVE_ACC, player2.getAcc().y));
	else player2.setAcc(glm::vec2(0, player2.getAcc().y));
}
