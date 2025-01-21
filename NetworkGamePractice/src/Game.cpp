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
	ball.setSpeed(glm::vec2(BALL_INITIAL_SPEED.x * (rand() % 2 == 0 ? 1 : -1), BALL_INITIAL_SPEED.y));
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
	player1.updateState(elapsedTime);

	player2.updatePosition(elapsedTime);
	player2.updateSpeed(elapsedTime);
	player2.updateState(elapsedTime);

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

	// ball - ceil
	if (checkCollision(ball, ceil))
	{
		ball.setSpeed(glm::vec2(ball.getSpeed().x, -ball.getSpeed().y));
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
		if(player1.getSpikeReserved())
		{
			switch (player1.getSpikeDirection())
			{
				case SpikeDirection::None:
				{
					ball.addImpulse(glm::vec2(1.f, 0.f));
				}
				break;
				case SpikeDirection::Front:
				{
					ball.addImpulse(glm::vec2(3.f, 0.f));
				}
				break;
				case SpikeDirection::Up:
				{
					ball.addImpulse(glm::vec2(0.3f, 3.f));
				}
				break;
				case SpikeDirection::Down:
				{
					ball.addImpulse(glm::vec2(0.3f, -3.f));
				}
				break;
				case SpikeDirection::Front_Up:
				{
					ball.addImpulse(glm::vec2(1.f, 3.f));
				}
				break;
				case SpikeDirection::Front_Down:
				{
					ball.addImpulse(glm::vec2(1.f, -3.f));
				}
				break;
				default:
				{
					std::cerr << "Unhandled Spike Direction!" << std::endl;
				}
			}
		}
		else
		{
			ball.setSpeed(glm::vec2(1.f, 1.5f));
		}
		return;
	}
	if (checkCollision(ball, player2))
	{
		if (player2.getSpikeReserved())
		{
			switch (player2.getSpikeDirection())
			{
				case SpikeDirection::None:
				{
					ball.addImpulse(glm::vec2(-1.f, 0.f));
				}
				break;
				case SpikeDirection::Front:
				{
					ball.addImpulse(glm::vec2(-3.f, 0.f));
				}
				break;
				case SpikeDirection::Up:
				{
					ball.addImpulse(glm::vec2(0.3f, 3.f));
				}
				break;
				case SpikeDirection::Down:
				{
					ball.addImpulse(glm::vec2(0.3f, -3.f));
				}
				break;
				case SpikeDirection::Front_Up:
				{
					ball.addImpulse(glm::vec2(-1.f, 3.f));
				}
				break;
				case SpikeDirection::Front_Down:
				{
					ball.addImpulse(glm::vec2(-1.f, -3.f));
				}
				break;
				default:
				{
					std::cerr << "Unhandled Spike Direction!" << std::endl;
				}
			}
		}
		else
		{
			ball.setSpeed(glm::vec2(1.f, 1.5f));
		}
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
	// player1
	static bool DPressed = false, GPressed = false, RPressed = false, FPressed = false;
	if (key == GLFW_KEY_D && action == GLFW_PRESS) DPressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) DPressed = false;
	if (key == GLFW_KEY_G && action == GLFW_PRESS) GPressed = true;
	if (key == GLFW_KEY_G && action == GLFW_RELEASE) GPressed = false;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) RPressed = true;
	if (key == GLFW_KEY_R && action == GLFW_RELEASE) RPressed = false;
	if (key == GLFW_KEY_F && action == GLFW_PRESS) FPressed = true;
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) FPressed = false;
	if(!player1.getSliding())
	{
		// jump
		if (key == GLFW_KEY_R && action == GLFW_PRESS && !player1.getJumping()) player1.jump();

		// move
		if (DPressed && GPressed) player1.setAcc(glm::vec2(0, player1.getAcc().y));
		else if (DPressed) player1.setAcc(glm::vec2(-PLAYER_MOVE_ACC, player1.getAcc().y));
		else if (GPressed) player1.setAcc(glm::vec2(PLAYER_MOVE_ACC, player1.getAcc().y));
		else player1.setAcc(glm::vec2(0, player1.getAcc().y));

		// slide
		if (key == GLFW_KEY_Z && action == GLFW_PRESS && !player1.getJumping())
		{
			if (DPressed) 
			{
				player1.slide(false);
				DPressed = GPressed = false;
			}
			else if (GPressed)
			{
				player1.slide(true);
				DPressed = GPressed = false;
			}
		}
	}
	if (player1.getJumping())
	{
		// spike direction
		if (!DPressed && !GPressed && !RPressed && !FPressed)
		{
			player1.setSpikeDirection(SpikeDirection::None);
		}
		else if (DPressed || GPressed)
		{
			if(RPressed) player1.setSpikeDirection(SpikeDirection::Front_Up);
			else if(FPressed) player1.setSpikeDirection(SpikeDirection::Front_Down);
			else player1.setSpikeDirection(SpikeDirection::Front);
		}
		else if(RPressed) player1.setSpikeDirection(SpikeDirection::Up);
		else if(FPressed) player1.setSpikeDirection(SpikeDirection::Down);
		
		// spike
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			player1.reserveSpike();
		}
	}

	// player2
	static bool leftPressed = false, rightPressed = false, upPressed = false, downPressed = false;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) leftPressed = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) leftPressed = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) rightPressed = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) rightPressed = false;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) upPressed = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE) upPressed = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) downPressed = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) downPressed = false;
	if(!player2.getSliding())
	{
		// jump
		if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2.getJumping()) player2.jump();

		// move
		if (leftPressed && rightPressed) player2.setAcc(glm::vec2(0, player2.getAcc().y));
		else if (leftPressed) player2.setAcc(glm::vec2(-PLAYER_MOVE_ACC, player2.getAcc().y));
		else if (rightPressed) player2.setAcc(glm::vec2(PLAYER_MOVE_ACC, player2.getAcc().y));
		else player2.setAcc(glm::vec2(0, player2.getAcc().y));

		// slide
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && !player2.getJumping())
		{
			if (leftPressed)
			{
				player2.slide(false);
				leftPressed = rightPressed = false;
			}
			else if (rightPressed)
			{
				player2.slide(true);
				leftPressed = rightPressed = false;
			}
		}
	}
	if (player2.getJumping())
	{
		// spike direction
		if (!leftPressed && !rightPressed && !upPressed && !downPressed)
		{
			player2.setSpikeDirection(SpikeDirection::None);
		}
		else if (leftPressed || rightPressed)
		{
			if (upPressed) player2.setSpikeDirection(SpikeDirection::Front_Up);
			else if (downPressed) player2.setSpikeDirection(SpikeDirection::Front_Down);
			else player2.setSpikeDirection(SpikeDirection::Front);
		}
		else if (upPressed) player2.setSpikeDirection(SpikeDirection::Up);
		else if (downPressed) player2.setSpikeDirection(SpikeDirection::Down);

		// spike
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		{
			player2.reserveSpike();
		}
	}
}
