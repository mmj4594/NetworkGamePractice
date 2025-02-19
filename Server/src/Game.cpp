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

	currentGameState = GameState::Playing;
	readyRound();
}

void Game::endPlay()
{
}

void Game::tick(float elapsedTime)
{
	if (currentRoundState != RoundState::Ready)
	{
		updatePosition(elapsedTime);
		updatePhysics(elapsedTime);
	}

	if (currentRoundState == RoundState::Ready)
	{
		roundWaitTimer += elapsedTime;
		if (roundWaitTimer >= ROUND_WAIT_TIME)
		{
			startRound();
		}
	}
	else if (currentRoundState == RoundState::End)
	{
		if (currentGameState != GameState::End)
		{
			roundEndTimer += elapsedTime;
			if (roundEndTimer >= ROUND_END_TIME)
			{
				readyRound();
			}
		}
	}
}

void Game::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool DPressed = false, GPressed = false, RPressed = false, FPressed = false;
	static bool leftPressed = false, rightPressed = false, upPressed = false, downPressed = false;

	if (currentGameState != GameState::Playing || currentRoundState != RoundState::Playing)
	{
		DPressed = false, GPressed = false, RPressed = false, FPressed = false;
		player1.setAcc(glm::vec2(0, player1.getAcc().y));

		leftPressed = false, rightPressed = false, upPressed = false, downPressed = false;
		player2.setAcc(glm::vec2(0, player2.getAcc().y));
		return;
	}

	// player1
	if (key == GLFW_KEY_D && action == GLFW_PRESS) DPressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) DPressed = false;
	if (key == GLFW_KEY_G && action == GLFW_PRESS) GPressed = true;
	if (key == GLFW_KEY_G && action == GLFW_RELEASE) GPressed = false;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) RPressed = true;
	if (key == GLFW_KEY_R && action == GLFW_RELEASE) RPressed = false;
	if (key == GLFW_KEY_F && action == GLFW_PRESS) FPressed = true;
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) FPressed = false;
	if (!player1.getSliding())
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
			if (RPressed) player1.setSpikeDirection(SpikeDirection::Front_Up);
			else if (FPressed) player1.setSpikeDirection(SpikeDirection::Front_Down);
			else player1.setSpikeDirection(SpikeDirection::Front);
		}
		else if (RPressed) player1.setSpikeDirection(SpikeDirection::Up);
		else if (FPressed) player1.setSpikeDirection(SpikeDirection::Down);

		// spike
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			player1.reserveSpike();
		}
	}

	// player2
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) leftPressed = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) leftPressed = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) rightPressed = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) rightPressed = false;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) upPressed = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE) upPressed = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) downPressed = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) downPressed = false;
	if (!player2.getSliding())
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

void Game::readyRound()
{
	currentRoundState = RoundState::Ready;
	currentTimeScale = BASIC_TIME_SCALE;
	player1.reset();
	player2.reset();
	ball.reset();
	roundWaitTimer = 0.f;

	std::cout << "Ready Round! Round Wait Time: " << ROUND_WAIT_TIME << std::endl;
}

void Game::startRound()
{
	currentRoundState = RoundState::Playing;
	currentTimeScale = BASIC_TIME_SCALE;
	std::cout << "Start Round!" << std::endl;
}

void Game::endRound()
{
	currentRoundState = RoundState::End;

	// game set
	if (scorePlayer1 >= MAX_SCORE || scorePlayer2 >= MAX_SCORE)
	{
		currentGameState = GameState::End;
	}
	else
	{
		currentTimeScale = ROUND_END_TIME_SCALE;
		roundEndTimer = 0.f;
	}

	std::cout << "Finish Round! Player1: " << scorePlayer1 << ", Player2: " << scorePlayer2 << std::endl << std::endl;
}

bool Game::checkCollision(GameObject obj1, GameObject obj2)
{
	return obj1.getPosition().x + obj1.getWidth() / 2 > obj2.getPosition().x - obj2.getWidth() / 2
		&& obj1.getPosition().x - obj1.getWidth() / 2 < obj2.getPosition().x + obj2.getWidth() / 2
		&& obj1.getPosition().y + obj1.getHeight() / 2 > obj2.getPosition().y - obj2.getHeight() / 2
		&& obj1.getPosition().y - obj1.getHeight() / 2 < obj2.getPosition().y + obj2.getHeight() / 2;
}

void Game::updatePosition(float elapsedTime)
{
	player1.updatePosition(elapsedTime);
	player1.updateSpeed(elapsedTime);
	player1.updateState(elapsedTime);

	player2.updatePosition(elapsedTime);
	player2.updateSpeed(elapsedTime);
	player2.updateState(elapsedTime);

	ball.updatePosition(elapsedTime);
	ball.updateSpeed(elapsedTime);
}

void Game::updatePhysics(float elapsedTime)
{
	// ball - floor
	if (checkCollision(ball, floor))
	{
		if (currentGameState == GameState::Playing && currentRoundState == RoundState::Playing)
		{
			if (ball.getPosition().x < SCREEN_WIDTH / 2)
			{
				scorePlayer2++;
				lastRoundWinnerPlayerID = player2.getPlayerID();
			}
			else
			{
				scorePlayer1++;
				lastRoundWinnerPlayerID = player1.getPlayerID();
			}
			endRound();
			return;
		}

		ball.setPosition(glm::vec2(ball.getPosition().x, floor.getTop() + ball.getHeight() / 2));
		ball.setSpeed(glm::vec2(ball.getSpeed().x, std::abs(ball.getSpeed().y)));
	}

	// ball - ceil
	if (checkCollision(ball, ceil))
	{
		ball.setPosition(glm::vec2(ball.getPosition().x, ceil.getBottom() - ball.getHeight() / 2));
		ball.setSpeed(glm::vec2(ball.getSpeed().x, 0.f));
	}

	// ball - net
	if (checkCollision(ball, net))
	{
		float overlapX = std::min(ball.getRight(), net.getRight()) - std::max(ball.getLeft(), net.getLeft());
		float overlapY = std::min(ball.getTop(), net.getTop()) - std::max(ball.getBottom(), net.getBottom());
		if (overlapX > overlapY)
		{
			ball.setPosition(glm::vec2(ball.getPosition().x, net.getTop() + ball.getHeight() / 2));
			ball.setSpeed(glm::vec2(ball.getSpeed().x, std::abs(ball.getSpeed().y)));
		}
		else if (net.getPosition().x < ball.getPosition().x)
		{
			ball.setPosition(glm::vec2(net.getRight() + ball.getWidth() / 2, ball.getPosition().y));
			ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		}
		else if (net.getPosition().x > ball.getPosition().x)
		{
			ball.setPosition(glm::vec2(net.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
			ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		}
	}

	// ball - player1
	if (checkCollision(ball, player1))
	{
		if (ball.getIgnorePlayerCollision() == false)
		{
			// position
			{
				float overlapX = std::min(ball.getRight(), player1.getRight()) - std::max(ball.getLeft(), player1.getLeft());
				float overlapY = std::min(ball.getTop(), player1.getTop()) - std::max(ball.getBottom(), player1.getBottom());
				if (overlapX > overlapY)
				{
					ball.setPosition(glm::vec2(ball.getPosition().x, player1.getTop() + ball.getHeight() / 2));
				}
				else if (player1.getPosition().x < ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player1.getRight() + ball.getWidth() / 2, ball.getPosition().y));
				}
				else if (player1.getPosition().x > ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player1.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
				}
			}

			// speed
			{
				glm::vec2 newBallSpeed = ball.getSpeed();

				// spike
				if (player1.getSpikeReserved())
				{
					switch (player1.getSpikeDirection())
					{
					case SpikeDirection::None:
						newBallSpeed.x = BALL_SPIKE_SPEED;
						newBallSpeed.y = 0.f;
						break;
					case SpikeDirection::Front:
						newBallSpeed.x = BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = 0.f;
						break;
					case SpikeDirection::Up:
						newBallSpeed.x = BALL_SPIKE_SPEED;
						newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Down:
						newBallSpeed.x = BALL_SPIKE_SPEED;
						newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Front_Up:
						newBallSpeed.x = BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Front_Down:
						newBallSpeed.x = BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
						break;
					default:
						std::cerr << "Unhandled Spike Direction!" << std::endl;
					}
				}
				// normal collision
				else
				{
					newBallSpeed.x = NORMAL_COLLISION_IMPACT_FACTOR * (ball.getPosition().x - player1.getPosition().x);
					newBallSpeed.y = std::abs(ball.getSpeed().y);
				}
				ball.setSpeed(newBallSpeed);
			}
			ball.setIgnorePlayerCollision(true);
		}
	}
	// ball - player2
	else if (checkCollision(ball, player2))
	{
		if (ball.getIgnorePlayerCollision() == false)
		{
			// position
			{
				float overlapX = std::min(ball.getRight(), player2.getRight()) - std::max(ball.getLeft(), player2.getLeft());
				float overlapY = std::min(ball.getTop(), player2.getTop()) - std::max(ball.getBottom(), player2.getBottom());
				if (overlapX > overlapY)
				{
					ball.setPosition(glm::vec2(ball.getPosition().x, player2.getTop() + ball.getHeight() / 2));
				}
				else if (player2.getPosition().x < ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player2.getRight() + ball.getWidth() / 2, ball.getPosition().y));
				}
				else if (player2.getPosition().x > ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player2.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
				}
			}

			// speed
			{
				glm::vec2 newBallSpeed = ball.getSpeed();

				// spike
				if (player2.getSpikeReserved())
				{
					switch (player2.getSpikeDirection())
					{
					case SpikeDirection::None:
						newBallSpeed.x = -BALL_SPIKE_SPEED;
						newBallSpeed.y = 0.f;
						break;
					case SpikeDirection::Front:
						newBallSpeed.x = -BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = 0.f;
						break;
					case SpikeDirection::Up:
						newBallSpeed.x = -BALL_SPIKE_SPEED;
						newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Down:
						newBallSpeed.x = -BALL_SPIKE_SPEED;
						newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Front_Up:
						newBallSpeed.x = -BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
						break;
					case SpikeDirection::Front_Down:
						newBallSpeed.x = -BALL_SPIKE_SPEED * 2;
						newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
					default:
						std::cerr << "Unhandled Spike Direction!" << std::endl;
					}
				}
				// normal collision
				else
				{
					newBallSpeed.x = NORMAL_COLLISION_IMPACT_FACTOR * (ball.getPosition().x - player2.getPosition().x);
					newBallSpeed.y = std::abs(ball.getSpeed().y);
				}
				ball.setSpeed(newBallSpeed);
			}
			ball.setIgnorePlayerCollision(true);
		}
	}
	else
	{
		ball.setIgnorePlayerCollision(false);
	}

	// ball - wall
	if (checkCollision(ball, leftWall))
	{
		ball.setPosition(glm::vec2(leftWall.getRight() + ball.getWidth() / 2, ball.getPosition().y));
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
	}
	if (checkCollision(ball, rightWall))
	{
		ball.setPosition(glm::vec2(rightWall.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
	}
}
